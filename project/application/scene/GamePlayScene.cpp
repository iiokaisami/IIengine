#include "GamePlayScene.h"

#include <ModelManager.h>
#include <Ease.h>
#include <corecrt_math_defines.h>

#include "TimeManager.h"

using namespace IIEngine;

void GamePlayScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	// カメラの作成
	camera_ = std::make_unique<SceneCamera>();
	camera_->Initialize();

	cameraRotate_ = camera_->GetRotate();
	cameraPosition_ = camera_->GetPosition();
	cameraPosition_.y = 70.0f;
	cameraPosition_.z = -15.0f;
	cameraRotate_.x = 1.2f;

	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(camera_->GetCamera());

	camera_->SetPosition(camStart_);
	camera_->SetRotate(camStartRot_);

	// ポーズメニュー
	pauseMenu_ = std::make_unique<PauseMenu>();
	pauseMenu_->Initialize();

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

	// スプライト
	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		auto sprite = std::make_unique<Sprite>();

		if (i == 0)
		{
			sprite->Initialize("playUI.png", uiPos_ , color_, { 0,0 });
		}
		if (i == 1)
		{
			sprite->Initialize("playSPACE.png", spacePos_, color_, { 0,0 });
		}
		if (i == 2)
		{
			sprite->Initialize("playW.png", wPos_, color_, { 0,0 });
		}
		if (i == 3)
		{
			sprite->Initialize("playA.png", aPos_, color_, { 0,0 });
		}
		if (i == 4)
		{
			sprite->Initialize("playS.png", sPos_, color_, { 0,0 });
		}
		if (i == 5)
		{
			sprite->Initialize("playD.png", dPos_, color_, { 0,0 });
		}

		sprites_.push_back(std::move(sprite));
	}

	// レベルデータの読み込み
	levelData_ = IIEngine::LevelDataLoader::LoadLevelData("wallSetting");
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

	// 壁データの収集
	std::vector<Vector3> wallPositions;
	for (const auto& wall : pWalls_)
	{
		// 壁の位置をリストに追加
		wallPositions.push_back(wall->GetPosition());
	}
	// エネミーマネージャーに壁データを渡す
	pEnemyManager_->SetObstacles(wallPositions);


	// カメラの追従
	camera_->AddController(std::make_unique<FollowController>(
		camera_->GetCamera(),
		[this]() -> Vector3 { return pPlayer_->GetPosition(); }, // プレイヤー位置を供給
		[this]() -> Vector3 { return pPlayer_->GetVelocity(); }, // プレイヤー速度を供給
		followSpeed_,// 追従の速さ
		inputLead_, // 先行入力の重み
		cameraOffset_
	));

	// カメラのシェイク
	camera_->AddController(std::make_unique<ShakeController>(
		camera_->GetCamera(),
		[this]() -> bool
		{
			bool hit = pPlayer_->IsHitMoment();
			if (hit) pPlayer_->SetHitMoment(false); // consume
			return hit;
		},
		duration_,
		amplitude_
	));

	// スタート用コントローラ作成
	camera_->AddController<StartCameraController>(
		camera_->GetCamera(),
		camStart_, camControl1_, camControl2_, camEnd_,
		camStartRot_, camEndRot_,
		cameraStartDuration_);

	// デスカメラコントローラ作成
	camera_->AddController<DeathCameraController>(
		camera_->GetCamera(),
		[this]() -> Vector3 { return pPlayer_->GetPosition(); },
		deathCameraDuration_, deathCameraRotations_, deathEndRadius_, deathEndHeight_);

	// クリアカメラコントローラ作成
	camera_->AddController<ClearCameraController>(
		camera_->GetCamera(),
		[this]() -> Vector3 { return pPlayer_->GetPosition(); },
		clearCameraDuration_, clearCameraRotations_, rotationSpeedMultiplier_, finalDistance_, finalHeight_);

	// バリア破壊カメラコントローラ作成
	camera_->AddController<BarrierBreakCameraController>(
		camera_->GetCamera(),
		[this]() -> Vector3 { return pPlayer_->GetPosition(); }, // Player
		[this]() -> Vector3 { return pGoal_->GetPosition(); },   // Goal
		[this]() -> Vector3 { return pPlayer_->GetPosition() + cameraOffset_; }, // 目標位置(Playerの位置にオフセット)
		[this]() -> bool { return pGoal_->IsBarrierDestroyed(); } // Barrier破壊完了
	);

	// コールバック設定
	if (auto* barrierCam = camera_->FindController<BarrierBreakCameraController>())
	{
		// Goal到達＝ここでバリア破壊開始
		barrierCam->SetOnArriveGoal([this]()
			{
				if (pGoal_) pGoal_->SetBarrierDestroyed(true);
			});

		// 演出終了＝Follow復帰
		barrierCam->SetOnFinish([this]()
			{
				if (auto* follow = camera_->FindController<FollowController>())
				{
					follow->SnapToTarget();
					follow->Start();
				}
			});
	}

	// コールバック設定(FindController を使ってその場で取得)
	if (auto* death = camera_->FindController<DeathCameraController>())
	{
		death->SetOnFinish([this]()
			{
			if (pPlayer_)
			{
				pPlayer_->StartDeathMotion();
			}
		
			});
	}

	if (auto* clear = camera_->FindController<ClearCameraController>())
	{
		clear->SetOnFadeStart([this]() { isClearFadeStart_ = true; });
		clear->SetOnFinish([this]()
			{
			if (auto* follow = camera_->FindController<FollowController>()) follow->Start();
			});
	}

	if (auto* startCtrl = camera_->FindController<StartCameraController>())
	{
		startCtrl->SetOnFinish([this]()
			{
				// スタート演出終わったらフォロー再開
				if (auto* follow = camera_->FindController<FollowController>())
				{
					follow->SnapToTarget();
					follow->Start();
				}
				// フラグがあればクリア
				isStartCamera_ = false;
			});

		// 演出前に追尾を止めておく
		if (auto* follow = camera_->FindController<FollowController>())
		{
			follow->Stop();
		}
	}

	// シーン開始時遷移演出
	blockTransition_ = std::make_unique<IIEngine::BlockRiseTransition>(IIEngine::BlockRiseTransition::Mode::DropOnly);
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


	// RGBシフトエフェクトの強さを0にしておく
	PostEffectManager::GetInstance()->GetPassAs<RGBShiftPass>("RGBShift")->SetIntensity(0.0f);

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

	// Object3dCommon に設定したデフォルトカメラを解除
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(nullptr);

	// SceneCamera
	if (camera_)
	{
		camera_->Finalize();
		camera_.reset();
	}
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
			// 初回開始用フラグが立っていればトランジション完了時にスタートカメラを開始する
			if (isStartCamera_)
			{
				if (auto* startCtrl = camera_->FindController<StartCameraController>())
				{
					pPlayer_->SetTouchGoal(false);
					startCtrl->Start();
				}
			}

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
	
	// ポーズ入力
	if (inputLock_)
	{
		if (!IIEngine::Input::GetInstance()->PushKey(DIK_SPACE))
		{
			inputLock_ = false;
		}
		return;
	}

	if (IIEngine::Input::GetInstance()->TriggerKey(DIK_ESCAPE))
	{
		if (!isPaused_)
		{
			isPaused_ = true;
			pauseMenu_->Open();
		} 
		else
		{
			isPaused_ = false;
			pauseMenu_->Close();
		}
	}
	// ポーズ中は更新しない
	if (isPaused_)
	{
		pauseMenu_->Update();

		switch (pauseMenu_->ConsumeSelectedOption())
		{
		case PauseMenu::MenuOption::Resume:

			isPaused_ = false;
			pauseMenu_->Close();
			inputLock_ = true;
			return;

		case PauseMenu::MenuOption::Restart:

			// トランジション開始
			blockTransition_ = std::make_unique<IIEngine::BlockRiseTransition>();
			isTransitioning_ = true;
			blockTransition_->Start([]
				{
					// シーン切り替え
					IIEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
				});
			break;

		case PauseMenu::MenuOption::ReturnTitle:
			
			// トランジション開始
			blockTransition_ = std::make_unique<IIEngine::BlockRiseTransition>();
			isTransitioning_ = true;
			blockTransition_->Start([]
				{
					// シーン切り替え
					IIEngine::SceneManager::GetInstance()->ChangeScene("TITLE");
				});

			break;

		default:
			break;
		}

		return;
	}


	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	{
		// sprite の順序: space, W, A, S, D の 5 つを順番に動かす
		static float seqTimer = 0.0f;
		constexpr int kCount = 5;
		const float slotDuration = 0.2f; // 各スロットの所要時間
		const float totalDuration = slotDuration * kCount;

		seqTimer += dt;
		// 0..totalDuration に折り返す
		if (seqTimer >= totalDuration) seqTimer = std::fmod(seqTimer, totalDuration);

		auto calcY = [&](int index) -> float
			{
				const float tLocal = seqTimer - (index * slotDuration);
				// 有効範囲
				if (tLocal < 0.0f || tLocal >= slotDuration) return 0.0f;
				// 進行度
				const float p = tLocal / slotDuration;
				return -3.0f * std::sinf(p * static_cast<float>(M_PI));
			};

		spacePos_.y = calcY(0);
		wPos_.y = calcY(1);
		aPos_.y = calcY(2);
		sPos_.y = calcY(3);
		dPos_.y = calcY(4);

		if (sprites_.size() >= 6)
		{
			sprites_[1]->SetPosition({ spacePos_.x, spacePos_.y });
			sprites_[2]->SetPosition({ wPos_.x, wPos_.y });
			sprites_[3]->SetPosition({ aPos_.x, aPos_.y });
			sprites_[4]->SetPosition({ sPos_.x, sPos_.y });
			sprites_[5]->SetPosition({ dPos_.x, dPos_.y });
		}
	}

	// 当たり判定チェック
	colliderManager_->CheckAllCollision();

	// プレイヤーの更新
	pPlayer_->Update();
	// プレイヤーの位置をエネミーマネージャーにセット
	pEnemyManager_->SetPlayerPosition(pPlayer_->GetPosition());

	// デス演出トリガー
	if (pPlayer_->IsDead() && !pPlayer_->IsAutoControl() && !isTransitioning_ && !isDeadCameraPlayer_)
	{
		// 一度だけ開始フラグを立てる
		isDeadCameraPlayer_ = true;
		isDeathCamera_ = true;

		// 追尾を止める
		if (auto* follow = camera_->FindController<FollowController>())
		{
			follow->Stop();
		}
		// デスカメラ開始
		if (auto* death = camera_->FindController<DeathCameraController>())
		{
			death->Start();
		}
	}

	// クリア演出トリガー
	if (pGoal_->IsCleared() && !isTransitioning_ && !isClearMoment_)
	{
		isClearMoment_ = true;
		isClearCamera_ = true;

		// 追尾を止める
		if (auto* follow = camera_->FindController<FollowController>())
		{
			follow->Stop();
		}

		// クリアカメラ開始
		if (auto* clear = camera_->FindController<ClearCameraController>())
		{
			clear->Start();
		}
	}

	// カメラの更新(シェイク、追尾、引き)
	camera_->Update(dt);
	// カメラマネージャーの更新
	cameraManager.UpdateAll();

	// プレイヤーのHPバー更新(カメラの更新後に呼び出したい)
	pPlayer_->UpdateHPBar();

	// エネミーの更新
	pEnemyManager_->Update();
	// 最も近いエネミーの位置を取得してプレイヤーにセット
	Vector3 nearest;
	if (pEnemyManager_->GetNearestEnemyPosition(pPlayer_->GetPosition(), nearest)) 
	{
		pPlayer_->SetTargetEnemyPosition(nearest);
	}
	else 
	{
		pPlayer_->ClearTargetEnemy();
	}

	// フィールドの更新
	pField_->Update();

	// 壁の更新
	for (auto& wall : pWalls_)
	{
		wall->Update();
	}

	// ゴールの更新
	pGoal_->Update();
	
	// 敵全滅した瞬間に一度だけカメラ演出開始
	if (pEnemyManager_->IsAllEnemyDefeated() && !isBarrierCameraStarted_)
	{
		isBarrierCameraStarted_ = true;

		// Follow停止
		if (auto* follow = camera_->FindController<FollowController>())
		{
			follow->Stop();
		}

		// Barrier破壊カメラ開始
		if (auto* barrierCam = camera_->FindController<BarrierBreakCameraController>())
		{
			barrierCam->Start();
		}
	}

	// スプライトの更新
	for (auto& sprite : sprites_)
	{
		sprite->Update();

		sprite->SetColor(color_);
	}


#ifdef USE_IMGUI

	AllImGui();

	if (IIEngine::Input::GetInstance()->TriggerKey(DIK_UP))
	{
		// トランジション開始
		fadeTransition_ = std::make_unique<IIEngine::FadeTransition>();
		isTransitioning_ = true;
		fadeTransition_->Start([]
			{
				// シーン切り替え
				IIEngine::SceneManager::GetInstance()->ChangeScene("CLEAR");
			});
	}
	if (IIEngine::Input::GetInstance()->TriggerKey(DIK_DOWN))
	{
		// トランジション開始
		blockTransition_ = std::make_unique<IIEngine::BlockRiseTransition>();
		isTransitioning_ = true;
		blockTransition_->Start([]
			{
				// シーン切り替え
				IIEngine::SceneManager::GetInstance()->ChangeScene("GAMEOVER");
			});
	}

#endif // USE_IMGUI


	if (isClearFadeStart_ && !isTransitioning_)
	{
		// トランジション開始
		fadeTransition_ = std::make_unique<IIEngine::FadeTransition>();
		isTransitioning_ = true;
		fadeTransition_->Start([]
			{
			// シーン切り替え
				IIEngine::SceneManager::GetInstance()->ChangeScene("CLEAR");
			});
	}
	if (pPlayer_->IsDeathMotionComplete() && !isTransitioning_)
	{
		// トランジション開始
		blockTransition_ = std::make_unique<IIEngine::BlockRiseTransition>();
		isTransitioning_ = true;
		blockTransition_->Start([]
			{
				// シーン切り替え
				IIEngine::SceneManager::GetInstance()->ChangeScene("GAMEOVER");
			});
	}
}

void GamePlayScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

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

	pPlayer_->Draw2D();

	pEnemyManager_->Draw2D();

	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}

	// ポーズの描画
	if (isPaused_)
	{
		pauseMenu_->Draw();
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

void GamePlayScene::AllImGui()
{
#ifdef USE_IMGUI

	ImGui::Begin("PlayScene");


	// 透明度の更新
	ImGui::SliderFloat4("SpriteColor", &color_.x, 0.0f, 1.0f);

	// camera
	Vector3 cam1Pos = camera_->GetPosition();
	Vector3 cam1Rot = camera_->GetRotate();
	if (ImGui::SliderFloat3("cameraPosition", &cam1Pos.x, -100.0f, 100.0f))
	{
		camera_->SetPosition(cam1Pos);
	}
	if (ImGui::SliderFloat3("cameraRotate", &cam1Rot.x, -10.0f, 10.0f))
	{
		camera_->SetRotate(cam1Rot);
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
}