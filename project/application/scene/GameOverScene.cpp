#include "GameOverScene.h"

#include <Ease.h>

void GameOverScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	camera_ = std::make_shared<Camera>();
	camera_->SetRotate({ 0.3f,0.0f,0.0f });
	camera_->SetPosition({ 0.0f,4.0f,-40.0f });
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_);
	cameraManager.AddCamera(camera_);
	cameraManager.SetActiveCamera(0);

	cameraPosition_ = { 0.0f,20.0f,-50.0f };
	cameraRotate_ = { 0.3f,0.0f,0.0f };
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		Sprite* sprite = new Sprite();
		
		if (i == 0)
		{
			sprite->Initialize("gameOverLogo.png", { -10,0 }, {1.0f,1.0f,1.0f,1.0f}, { 0,0 });
		}
		else if (i == 1)
		{
			sprite->Initialize("gameOverReTry.png", { 0,620 }, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		}
		else if (i == 2)
		{
			sprite->Initialize("gameOverToTitle.png", { -70,620 }, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		}

		sprites.push_back(sprite);
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

	// シーン開始時にフェードイン
	transition_ = std::make_unique<BlockRiseTransition>(BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	transition_->Start(nullptr);
}

void GameOverScene::Finalize()
{
	pPlayer_->Finalize();
	pEnemyManager_->Finalize();

	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}

	cameraManager.RemoveCamera(0);
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

	camera_->Update();
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);	

	for (Sprite* sprite : sprites)
	{
		sprite->Update();

	}

	// 当たり判定チェック
	colliderManager_->CheckAllCollision();

	// プレイヤーの更新
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


	if (Input::GetInstance()->TriggerKey(DIK_RETURN))
	{
		// トランジション開始
		transition_ = std::make_unique<BlockRiseTransition>();
		isTransitioning_ = true;
		transition_->Start([]
			{
				// シーン切り替え
				SceneManager::GetInstance()->ChangeScene("TITLE");
			});
	}

	if( Input::GetInstance()->TriggerKey(DIK_R))
	{
		// トランジション開始
		transition_ = std::make_unique<BlockRiseTransition>();
		isTransitioning_ = true;
		transition_->Start([]
			{
				// シーン切り替え
				SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
			});
	}

}

void GameOverScene::Draw()
{
	// 描画前処理(Object)
	Object3dCommon::GetInstance()->CommonDrawSetting();

	pPlayer_->Draw();

	pEnemyManager_->Draw();

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	for (Sprite* sprite : sprites)
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