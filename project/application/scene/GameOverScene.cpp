#include "GameOverScene.h"

#include <Ease.h>

#include "TimeManager.h"

using namespace IIEngine;

void GameOverScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	camera_ = std::make_unique<SceneCamera>();
	camera_->Initialize();
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(camera_->GetCamera());
	cameraManager.AddCamera(camera_->GetCamera());
	cameraManager.SetActiveCamera(0);
	
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		auto sprite = std::make_unique<Sprite>();
		
		if (i == 0)
		{
			sprite->Initialize("gameOverLogo.png", { 0,0 }, {1.0f,1.0f,1.0f,1.0f}, { 0,0 });
		}
		else if (i == 1)
		{
			sprite->Initialize("reTry.png", rePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		}
		else if (i == 2)
		{
			sprite->Initialize("toTitle.png", toPos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		}

		sprites_.push_back(std::move(sprite));
	}


	// 衝突判定
	colliderManager_ = ColliderManager::GetInstance();
	colliderManager_->Initialize();

	// プレイヤー
	pPlayer_ = std::make_unique<Player>();
	pPlayer_->Initialize();
	pPlayer_->SetPosition({ 0.0f,0.0f,0.0f });
	pPlayer_->SetIsCanMove(false); // 移動不可

	// エネミー
	pEnemyManager_ = std::make_unique<EnemyManager>();
	pEnemyManager_->GameOverEnemyInit();


	// カメラのシェイク
	camera_->AddController(std::make_unique<ShakeController>(
		camera_->GetCamera(),
		[this]() -> bool {
			bool hit = pPlayer_->IsHitMoment();
			if (hit) pPlayer_->SetHitMoment(false); // consume
			return hit;
		},
		0.3f, // duration
		0.5f  // amplitude
	));

	// シーン開始時にフェードイン
	transition_ = std::make_unique<IIEngine::BlockRiseTransition>(IIEngine::BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	transition_->Start(nullptr);

}

void GameOverScene::Finalize()
{
	pPlayer_->Finalize();
	pEnemyManager_->Finalize();

	// Object3dCommon に設定したデフォルトカメラを解除
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(nullptr);

	// SceneCamera
	if (camera_)
	{
		camera_->Finalize();
		camera_.reset();
	}
}

void GameOverScene::Update()
{
	// トランジション更新
	if (isTransitioning_ && transition_)
	{
		transition_->Update();

		// トランジション終了判定
		if (transition_->IsFinished())
		{
			transition_.reset();
			isTransitioning_ = false;
		}

	}

	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// カメラ更新
	camera_->Update(dt);
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	// rePos_とtoPos_を範囲で往復させる
	{
		static float timer = 0.0f;
		const float speed = 2.0f; // 速さ
		const float minX = 0.0f;
		const float maxX = 30.0f;
		const float mid = (minX + maxX) * 0.5f;
		const float amp = (maxX - minX) * 0.5f;

		timer += dt;
		const float x1 = mid + amp * std::sinf(timer * speed);
		const float x2 = mid + amp * std::sinf(timer * speed + 3.14159265358979f); // 逆位相

		rePos_.x = x1;
		toPos_.x = x2;
	}
	sprites_[1]->SetPosition(rePos_);
	sprites_[2]->SetPosition(toPos_);


	for (auto& sprite : sprites_)
	{
		sprite->Update();

	}

	// 当たり判定チェック
	colliderManager_->CheckAllCollision();

	// プレイヤーの更新
	pPlayer_->SetIsNoiseActive(false); // ゲームオーバーシーンではノイズを常にオフにする
	pPlayer_->Update();
	// プレイヤーの位置をエネミーマネージャーにセット
	pEnemyManager_->SetPlayerPosition(pPlayer_->GetPosition());

	// カメラの更新(シェイク、追尾、引き)
	if (!pPlayer_->IsDead())
	{
		camera_->SetRotate(cameraRotate_);
		camera_->SetPosition(cameraPosition_);
		CameraShake();
	}

	// エネミーの更新
	pEnemyManager_->GameOverEnemyUpdate();



#ifdef USE_IMGUI

	// 透明度の更新

	ImGui::Begin("GameOverScene");

	//ImGui::SliderFloat4("transparent", &color_.x, 0.0f, 1.0f);

	ImGui::SliderFloat3("cameraPosition", &cameraPosition_.x, -50.0f, 20.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate_.x, -3.14f, 3.14f);

	
	ImGui::End();

	pEnemyManager_->ImGuiDraw();

#endif // USE_IMGUI


	if (IIEngine::Input::GetInstance()->TriggerKey(DIK_RETURN) && !isTransitioning_ or
	(IIEngine::Input::GetInstance()->IsPadConnected() &&
		IIEngine::Input::GetInstance()->PushPadButton(ControllerButtonType::A)))
	{
		// トランジション開始
		transition_ = std::make_unique<IIEngine::BlockRiseTransition>();
		isTransitioning_ = true;
		transition_->Start([]
			{
				// シーン切り替え
				IIEngine::SceneManager::GetInstance()->ChangeScene("TITLE");
			});
	}

	if(IIEngine::Input::GetInstance()->TriggerKey(DIK_R) && !isTransitioning_ or
		(IIEngine::Input::GetInstance()->IsPadConnected() &&
			IIEngine::Input::GetInstance()->PushPadButton(ControllerButtonType::B)))
	{
		// トランジション開始
		transition_ = std::make_unique<IIEngine::BlockRiseTransition>();
		isTransitioning_ = true;
		transition_->Start([]
			{
				// シーン切り替え
				IIEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
			});
	}

}

void GameOverScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	pPlayer_->Draw();

	pEnemyManager_->Draw();

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}


	// トランジション描画
	if (isTransitioning_ && transition_)
	{
		transition_->Draw();
	}
}

void GameOverScene::CameraShake()
{
	// アクティブカメラの情報を取得
	auto activeCamera = cameraManager.GetActiveCamera();

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