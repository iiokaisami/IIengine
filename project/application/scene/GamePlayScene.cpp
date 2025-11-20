#include "GamePlayScene.h"

#include <ModelManager.h>
#include <Ease.h>
#include <corecrt_math_defines.h>

void GamePlayScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	// カメラの作成
	camera = std::make_shared<Camera>();

	cameraRotate = camera->GetRotate();
	cameraPosition = camera->GetPosition();
	cameraPosition.y = 70.0f;
	cameraPosition.z = -15.0f;
	cameraRotate.x = 1.2f;

	camera->SetPosition(camStart_);
	camera->SetRotate(camStartRot_);

	// カメラの追加
	cameraManager.AddCamera(camera);

	// アクティブカメラの設定
	cameraManager.SetActiveCamera(0);
	activeIndex = cameraManager.GetActiveIndex();

	Object3dCommon::GetInstance()->SetDefaultCamera(cameraManager.GetActiveCamera());

	// 衝突判定
	colliderManager_ = ColliderManager::GetInstance();
	colliderManager_->Initialize();

	// プレイヤー
	pPlayer_ = std::make_unique<Player>();
	pPlayer_->Initialize();

	// エネミー
	pEnemyManager_ = std::make_unique<EnemyManager>();
	pEnemyManager_->Initialize();

	// フィールド
	pField_ = std::make_unique<Field>();
	pField_->Initialize();

	// ゴールの初期化
	pGoal_ = std::make_unique<Goal>();
	pGoal_->Initialize();

	// 追尾の初期化
	cameraIsResting_ = true;
	cameraRestCenter_ = pPlayer_->GetPosition() + Vector3{ 0.0f,70.0f,-20.0f };

	// スプライト
	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		Sprite* sprite = new Sprite();

		if (i == 0)
		{
			sprite->Initialize("playUI.png", { 0,600 }, color_, { 0,0 });
		}

		sprites.push_back(sprite);
	}

	// レベルデータの読み込み
	levelData_ = LevelDataLoader::LoadLevelData("wallSetting");
	if (levelData_)
	{
		// レベルデータに基づいて壁を配置
		for (const auto& wallData : levelData_->walls)
		{
			auto wall = std::make_unique<Wall>();
			wall->Initialize();
			wall->SetPosition(wallData.position);
			wall->SetRotation(wallData.rotation);
			wall->SetScale(wallData.scale);
			pWalls_.push_back(std::move(wall));
		}
	}

	// シーン開始時遷移演出
	blockTransition_ = std::make_unique<BlockRiseTransition>(BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	blockTransition_->Start(nullptr);

	// スタートカメラ演出
	isStartCamera_ = true;
	cameraStartTimer_ = 0.0f;

	// デスカメラフラグ初期化
	isDeathCamera_ = false;
	isDeadCameraPlayer_ = false;

	// クリアカメラフラグ初期化
	isClearMoment_ = false;
	isClearCamera_ = false;
	isClearFadeStart_ = false;

}

void GamePlayScene::Finalize()
{
	pPlayer_->Finalize();
	pEnemyManager_->Finalize();
	pField_->Finalize();
	for (auto& wall : pWalls_)
	{
		wall->Finalize();
	}
	pGoal_->Finalize();

	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
	sprites.clear();

	// カメラ解放
	cameraManager.RemoveCamera(0);
	cameraManager.RemoveCamera(1);
}

void GamePlayScene::Update()
{
	// トランジション更新
	if (isTransitioning_ && blockTransition_)
	{
		blockTransition_->Update();

		// トランジション終了判定
		if (blockTransition_->IsFinished())
		{
			blockTransition_.reset();
			isTransitioning_ = false;
		}

	}

	if(isTransitioning_ && fadeTransition_)
	{
		fadeTransition_->Update();
		// トランジション終了判定
		if (fadeTransition_->IsFinished())
		{
			fadeTransition_.reset();
			isTransitioning_ = false;
		}
	}
	
	// スタートカメラ演出
	StartCamera();

	// カメラマネージャーの更新
	cameraManager.UpdateAll();

	for (Sprite* sprite : sprites)
	{
		sprite->Update();

		sprite->SetColor(color_);

	}

	// P押してカメラ切り替え
	if (Input::GetInstance()->TriggerKey(DIK_P))
	{
		if (cameraManager.GetActiveIndex() == 0)
		{
			cameraManager.SetActiveCamera(1);

		}
		else if (cameraManager.GetActiveIndex() == 1)
		{
			cameraManager.SetActiveCamera(0);

		}
	}
	// 稼働中のカメラインデックス
	activeIndex = cameraManager.GetActiveIndex();

	// 当たり判定チェック
	colliderManager_->CheckAllCollision();

	// プレイヤーの更新
	pPlayer_->Update();
	// プレイヤーの位置をエネミーマネージャーにセット
	pEnemyManager_->SetPlayerPosition(pPlayer_->GetPosition());

	// カメラの更新(シェイク、追尾、引き)
	if (!isStartCamera_ && !pPlayer_->IsDead())
	{
		camera->SetRotate(cameraRotate);
		camera->SetPosition(cameraPosition);
		CameraUpdate();
	}

	// エネミーの更新
	pEnemyManager_->Update();

	// フィールドの更新
	pField_->Update();

	// 壁の更新
	for (auto& wall : pWalls_)
	{
		wall->Update();
	}

	// ゴールの更新
	pGoal_->Update();
	pGoal_->SetBarrierDestroyed(pEnemyManager_->IsAllEnemyDefeated());

	// プレイヤーが死んだらデスカメラを開始(1回だけ)
	if (pPlayer_->IsDead() && !pPlayer_->IsAutoControl() && !isTransitioning_ && !isDeadCameraPlayer_)
	{
		if (!isDeathCamera_)
		{
			StartDeathCamera();
		}
	}

	// DeathCamera更新(StartDeathCameraが呼ばれている場合)
	if (isDeathCamera_)
	{
		UpdateDeathCamera(1.0f / 60.0f);
	}

	// クリア更新
	ClearUpdate();


#ifdef USE_IMGUI

	ImGui::Begin("PlayScene");


	// 透明度の更新
	ImGui::SliderFloat4("SpriteColor", &color_.x, 0.0f, 1.0f);

	// camera
	Vector3 cam1Pos = camera->GetPosition();
	Vector3 cam1Rot = camera->GetRotate();
	if (ImGui::SliderFloat3("cameraPosition", &cam1Pos.x, -100.0f, 100.0f))
	{
		camera->SetPosition(cam1Pos);
	}
	if (ImGui::SliderFloat3("cameraRotate", &cam1Rot.x, -10.0f, 10.0f)) 
	{
		camera->SetRotate(cam1Rot);
	}


	ImGui::End();

	pPlayer_->ImGuiDraw();
	pEnemyManager_->ImGuiDraw();
	pField_->ImGuiDraw();
	for (auto& wall : pWalls_)
	{
		wall->ImGuiDraw();
	}
	pGoal_->ImGuiDraw();


#endif // USE_IMGUI


	if (Input::GetInstance()->TriggerKey(DIK_UP) or (isClearFadeStart_ && !isTransitioning_))
	{
		// トランジション開始
		fadeTransition_ = std::make_unique<FadeTransition>();
		isTransitioning_ = true;
		fadeTransition_->Start([]
			{
			// シーン切り替え
			SceneManager::GetInstance()->ChangeScene("CLEAR");
			});
	}
	if (Input::GetInstance()->TriggerKey(DIK_DOWN) or (pPlayer_->IsDeathMotionComplete() && !isTransitioning_))
	{
		// トランジション開始
		blockTransition_ = std::make_unique<BlockRiseTransition>();
		isTransitioning_ = true;
		blockTransition_->Start([]
			{
				// シーン切り替え
				SceneManager::GetInstance()->ChangeScene("GAMEOVER");
			});
	}
}

void GamePlayScene::Draw()
{
	// 描画前処理(Object)
	Object3dCommon::GetInstance()->CommonDrawSetting();

	pPlayer_->Draw();

	pEnemyManager_->Draw();

	pField_->Draw();

	for (auto& wall : pWalls_)
	{
		wall->Draw();
	}

	pGoal_->Draw();


	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	for (Sprite* sprite : sprites)
	{
		sprite->Draw();
	}


	// トランジション描画
	if (isTransitioning_ && blockTransition_)
	{
		blockTransition_->Draw();
	}
	if (isTransitioning_ && fadeTransition_)
	{
		fadeTransition_->Draw();
	}
}

void GamePlayScene::CameraUpdate()
{
	// カメラのシェイク
	CameraShake();

	// カメラの追従
	CameraFollow();

}

void GamePlayScene::CameraShake()
{
	// アクティブカメラの情報を取得
	auto activeCamera = cameraManager.GetActiveCamera();
	if (activeCamera)
	{
		auto viewMatrix = activeCamera->GetViewMatrix();
	}

	// プレイヤーがヒットした場合にカメラをシェイク
	if (pPlayer_->IsHitMoment())
	{
		// アクティブなカメラを取得
		if (activeCamera)
		{
			// カメラをシェイク (持続時間,振幅)
			activeCamera->StartShake(0.3f, 0.6f);

			// ヒットフラグをリセット
			pPlayer_->SetHitMoment(false);
		}
	}

	// シェイク
	if (activeCamera)
	{
		activeCamera->UpdateShake(1.0f / 60.0f);
	}
}

void GamePlayScene::CameraFollow()
{
	if (!camera or !pPlayer_) 
	{
		return;
	}

	Vector3 playerPos = pPlayer_->GetPosition();

	// 固定のオフセット（プレイヤーから見たカメラ位置）
	Vector3 offset = { 0.0f, 80.0f, -20.0f };  // Y: 高さ、Z: 後方

	// 追従先の位置・回転
	Vector3 targetPos = playerPos + offset;
	Vector3 targetRot = { 2.0f, 0.0f, 0.0f };  // やや下向き

	// 滑らかに補間して追従
	Vector3 currentPos = camera->GetPosition();
	Vector3 nextPos;
	nextPos.Lerp(currentPos, targetPos, 0.8f);

	Vector3 currentRot = camera->GetRotate();
	Vector3 nextRot;
	nextRot.Lerp(currentRot, targetRot, 0.25f);

	camera->SetPosition(nextPos);
	camera->SetRotate(nextRot);
}

void GamePlayScene::StartCamera()
{
	if (isStartCamera_ && !isTransitioning_)
	{
		cameraStartTimer_ += 1.0f / 60.0f;
		float t = std::clamp(cameraStartTimer_ / cameraStartDuration_, 0.0f, 1.0f);
		float t_eased = Ease::InOutQuad(t);

		// 補間
		Vector3 camPos = Bezier3(camStart_, camControl1_, camControl2_, camEnd_, t_eased);
		Vector3 camRot = Lerp(camStartRot_, camEndRot_, t_eased);
		camera->SetPosition(camPos);
		camera->SetRotate(camRot);

		if (t >= 1.0f)
		{
			isStartCamera_ = false; // 終了
			camera->SetPosition(camEnd_);
			camera->SetRotate(camEndRot_);
		}
	}

}

void GamePlayScene::StartDeathCamera()
{
	if (!camera or !pPlayer_)
	{
		return;
	}

	// 一度だけフラグ
	isDeadCameraPlayer_ = true;

	isDeathCamera_ = true;
	deathCameraTimer_ = 0.0f;

	// プレイヤー位置と現カメラ位置から開始角度・半径・高さを算出
	Vector3 playerPos = pPlayer_->GetPosition();
	Vector3 camPos = camera->GetPosition();

	float dx = camPos.x - playerPos.x;
	float dz = camPos.z - playerPos.z;
	// startAngle を atan2(dx, dz)の順で取る(Bezier等で使ったのと整合を取る)
	deathStartAngle_ = std::atan2(dx, dz);
	deathStartRadius_ = std::sqrt(dx * dx + dz * dz);
	deathStartHeight_ = camPos.y - playerPos.y;

	// 最終的にプレイヤーの正面は 0 に
	deathTargetAngleOffset_ = 0.0f;
}

void GamePlayScene::UpdateDeathCamera(float deltaTime)
{
	if (!isDeathCamera_ or (!camera || !pPlayer_))
	{
		return;
	}

	deathCameraTimer_ += deltaTime;
	float t = std::clamp(deathCameraTimer_ / deathCameraDuration_, 0.0f, 1.0f);

	// イージング
	float t_eased = Ease::InOutQuad(t);

	// 角度は start -> target を経由して rotations 周回させる
	// targetAngle = deathTargetAngleOffset_0: プレイヤー前方
	float targetAngle = deathTargetAngleOffset_;
	// 差分を符号付きで正規化しておき、さらに周回分を追加
	float delta = targetAngle - deathStartAngle_;
	// normalize delta to [-pi, pi]
	while (delta > (float)M_PI) delta -= 2.0f * (float)M_PI;
	while (delta < (float) - M_PI) delta += 2.0f * (float)M_PI;
	float totalAngularTravel = delta + deathCameraRotations_ * 2.0f * (float)M_PI;
	float angle = deathStartAngle_ + totalAngularTravel * t_eased;

	// 半径と高さを補間(start -> end)
	float radius = std::lerp(deathStartRadius_, deathEndRadius_, t_eased);
	float height = std::lerp(deathStartHeight_, deathEndHeight_, t_eased);

	// カメラ位置を計算(playerを中心に極座標から)
	Vector3 playerPos = pPlayer_->GetPosition();
	Vector3 camPos;
	camPos.x = playerPos.x + std::sin(angle) * radius;
	camPos.z = playerPos.z + std::cos(angle) * radius;
	camPos.y = playerPos.y + height;

	camera->SetPosition(camPos);

	// カメラの回転：プレイヤーを見る方向に向ける
	Vector3 dir = (playerPos - camPos);
	// yaw: y軸回転(左右)を atan2(dir.x, dir.z)
	float yaw = std::atan2(dir.x, dir.z);
	// pitch: x軸回転(上下)を atan2(-dir.y, sqrt(x^2+z^2))
	float horizontalDist = std::sqrt(dir.x * dir.x + dir.z * dir.z);
	float pitch = std::atan2(-dir.y, horizontalDist);

	Vector3 camRot = { pitch, yaw, 0.0f };
	camera->SetRotate(camRot);

	// 終了判定
	if (t >= 1.0f)
	{
		isDeathCamera_ = false;
		// 最終的にぴったり正面位置にセットして終了
		Vector3 finalPos;
		finalPos.x = playerPos.x + std::sin(targetAngle) * deathEndRadius_;
		finalPos.z = playerPos.z + std::cos(targetAngle) * deathEndRadius_;
		finalPos.y = playerPos.y + deathEndHeight_;
		camera->SetPosition(finalPos);

		Vector3 finalDir = (playerPos - finalPos);
		float finalYaw = std::atan2(finalDir.x, finalDir.z);
		float finalH = std::sqrt(finalDir.x * finalDir.x + finalDir.z * finalDir.z);
		float finalPitch = std::atan2(-finalDir.y, finalH);
		camera->SetRotate({ finalPitch, finalYaw, 0.0f });

		// プレイヤー死亡演出
		pPlayer_->StartDeathMotion();

	}
}

void GamePlayScene::ClearUpdate()
{
	if (pGoal_->IsCleared() && !isTransitioning_ && !isClearMoment_)
	{
		// 一度だけフラグ
		isClearMoment_ = true;

		isClearCamera_ = true;
		clearCameraTimer_ = -2.0f;

		// プレイヤー位置と現カメラ位置から開始角度・半径・高さを算出
		Vector3 playerPos = pPlayer_->GetPosition();
		Vector3 camPos = camera->GetPosition();

		float dx = camPos.x - playerPos.x;
		float dz = camPos.z - playerPos.z;
		// startAngle を atan2(dx, dz)の順で取る(Bezier等で使ったのと整合を取る)
		clearStartAngle_ = std::atan2(dx, dz);
		clearStartRadius_ = std::sqrt(dx * dx + dz * dz);
		clearStartHeight_ = camPos.y - playerPos.y;

		// 最終的にプレイヤーの正面は 0 に
		clearTargetAngleOffset_ = 0.0f;
	}


	if (isClearCamera_)
	{
		if (clearCameraTimer_ >= 0.0f)
		{

			clearCameraTimer_ += 1.0f / 60.0f;
			float t = std::clamp(clearCameraTimer_ / clearCameraDuration_, 0.0f, 1.0f);

			// イージング
			float t_eased = Ease::InOutQuad(t);

			// --- 回転速度をさらに落とす係数 小さくするほど遅く回る ---
			const float rotationSpeedMultiplier = 0.12f; // 調整可
			// --- カメラを上昇させる量 ---
			const float clearHeightRise = 30.0f; // 調整可 正の値で上昇

			// 角度は start -> target を経由して rotations 周回させる
			float targetAngle = clearTargetAngleOffset_;
			float delta = targetAngle - clearStartAngle_;
			// normalize delta to [-pi, pi]
			while (delta > (float)M_PI) delta -= 2.0f * (float)M_PI;
			while (delta < (float)-M_PI) delta += 2.0f * (float)M_PI;

			// 周回数に multiplier をかけて実効的な回転量を減らす（急激な回転を抑制）
			float rotationsScaled = clearCameraRotations_ * rotationSpeedMultiplier;
			float totalAngularTravel = delta + rotationsScaled * 2.0f * (float)M_PI;
			float angle = clearStartAngle_ + totalAngularTravel * t_eased;

			// 半径は収縮させない（開始時の半径を維持）
			const float radius = clearStartRadius_;

			// 高さは開始高さから徐々に上昇
			float height = clearStartHeight_ + clearHeightRise * t_eased;

			// カメラ位置を計算(playerを中心に極座標から)
			Vector3 playerPos = pPlayer_->GetPosition();
			Vector3 camPos;
			camPos.x = playerPos.x + std::sin(angle) * radius;
			camPos.z = playerPos.z + std::cos(angle) * radius;
			camPos.y = playerPos.y + height;

			camera->SetPosition(camPos);

			// カメラの回転：プレイヤーを見る方向に向ける
			Vector3 dir = (playerPos - camPos);
			// yaw: y軸回転(左右)を atan2(dir.x, dir.z)
			float yaw = std::atan2(dir.x, dir.z);
			// pitch: x軸回転(上下)を atan2(-dir.y, sqrt(x^2+z^2))
			float horizontalDist = std::sqrt(dir.x * dir.x + dir.z * dir.z);
			float pitch = std::atan2(-dir.y, horizontalDist);

			Vector3 camRot = { pitch, yaw, 0.0f };
			camera->SetRotate(camRot);

			// 終了仕切る前にフェード開始判定
			if (t >= 0.4f)
			{
				// 遷移演出開始
				isClearFadeStart_ = true;
			}
		}
		else
		{
			clearCameraTimer_ += 1.0f / 60.0f;
		}
	}
}
