#include "ClearScene.h"

void ClearScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	camera_ = std::make_shared<Camera>();
	camera_->SetRotate({ 0.3f,0.0f,0.0f });
	camera_->SetPosition({ 0.0f,4.0f,-40.0f });
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_);
	cameraManager.AddCamera(camera_);
	cameraManager.SetActiveCamera(0);

	cameraPosition_ = { 0.0f,12.0f,-33.0f };
	cameraRotate_ = { 0.3f,0.0f,0.0f };
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	//プレイヤー
	pPlayer_ = std::make_unique<Player>();
	pPlayer_->Initialize();

	// スプライト
	for (uint32_t i = 0; i < 3; ++i)
	{
		Sprite* sprite = new Sprite();
		
		if (i == 0)
		{
			sprite->Initialize("clearLogo.png", { -10,0 }, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
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

	// シーン開始時にフェードイン
	transition_ = std::make_unique<BlockRiseTransition>(BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	transition_->Start(nullptr);

}

void ClearScene::Finalize()
{
	pPlayer_->Finalize();

	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}

	cameraManager.RemoveCamera(0);
}

void ClearScene::Update()
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

	// カメラ更新
	camera_->Update();
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	// スプライト更新
	for (Sprite* sprite : sprites)
	{
		sprite->Update();

	}

	// プレイヤー更新
	pPlayer_->ClearSceneUpdate();

	// パーティクル
	ParticleEmitter::Emit("goal", particlePosition_, 1);
	ParticleEmitter::Emit("petalGroup", petalPosition_, 1);


#ifdef USE_IMGUI

	// 透明度の更新

	ImGui::Begin("ClearScene");


	ImGui::SliderFloat3("cameraPosition", &cameraPosition_.x, -80.0f, 20.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate_.x, -3.14f, 3.14f);

	ImGui::SliderFloat3("particlePosition", &petalPosition_.x, -50.0f, 50.0f);

	ImGui::End();

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

	if (Input::GetInstance()->TriggerKey(DIK_R))
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

void ClearScene::Draw()
{
	// 描画前処理(Object)
	Object3dCommon::GetInstance()->CommonDrawSetting();

	// プレイヤー描画
	pPlayer_->Draw();

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
