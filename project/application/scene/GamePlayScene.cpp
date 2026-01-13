#include "GamePlayScene.h"

#include <ModelManager.h>
#include <Ease.h>
#include <corecrt_math_defines.h>

#include "TimeManager.h"

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

	Object3dCommon::GetInstance()->SetDefaultCamera(camera_->GetCamera());

	camera_->SetPosition(camStart_);
	camera_->SetRotate(camStartRot_);

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
			sprite->Initialize("playUI.png", { 0,600 }, color_, { 0,0 });
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

	// カメラの追従
	camera_->AddController(std::make_unique<FollowController>(
		camera_->GetCamera(),
		[this]() -> Vector3 { return pPlayer_->GetPosition(); }, // プレイヤー位置を供給
		0.8f, 0.25f,
		Vector3{ 1.4f, 0.0f, 0.0f }
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
		0.3f, // duration
		0.5f  // amplitude
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
		clearCameraDuration_, clearCameraRotations_, 0.12f, 30.0f, 0.4f);

	// コールバック設定（FindController を使ってその場で取得）
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
		if (auto* follow = camera_->FindController<FollowController>()) follow->Stop();
		// 演出を開始
		startCtrl->Start();
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
	Object3dCommon::GetInstance()->SetDefaultCamera(nullptr);

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
	
	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

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
	pGoal_->SetBarrierDestroyed(pEnemyManager_->IsAllEnemyDefeated());

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

	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}

	pPlayer_->Draw2D();

	pEnemyManager_->Draw2D();

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