#include "ClearScene.h"

#include "TimeManager.h"

using namespace IIEngine;

void ClearScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	camera_ = std::make_unique<SceneCamera>();
	camera_->Initialize();
	camera_->SetRotate({ 0.3f,0.0f,0.0f });
	camera_->SetPosition({ 0.0f,4.0f,-40.0f });
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(camera_->GetCamera());
	cameraManager.AddCamera(camera_->GetCamera());
	cameraManager.SetActiveCamera(0);

	cameraPosition_ = { 0.0f,12.0f,-33.0f };
	cameraRotate_ = { 0.3f,0.0f,0.0f };
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	//プレイヤー
	pPlayer_ = std::make_unique<Player>();
	pPlayer_->Initialize();

	// フィールド
	pField_ = std::make_unique<Field>();
	pField_->Initialize();

	// スプライト
	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		auto sprite = std::make_unique<Sprite>();
		
		if (i == 0)
		{
			sprite->Initialize("clearLogo.png", { 0,0 }, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
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

	// シーン開始時にフェードイン
	transition_ = std::make_unique<IIEngine::BlockRiseTransition>(IIEngine::BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	transition_->Start(nullptr);

}

void ClearScene::Finalize()
{
	pPlayer_->Finalize();
	pField_->Finalize();

	// Object3dCommon に設定したデフォルトカメラを解除
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(nullptr);

	// SceneCamera
	if (camera_)
	{
		camera_->Finalize();
		camera_.reset();
	}
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


	// スプライト更新
	for (auto& sprite : sprites_)
	{
		sprite->Update();
	}

	// プレイヤー更新
	pPlayer_->ClearSceneUpdate();

	// フィールド更新
	pField_->Update();

	// パーティクル
	IIEngine::ParticleEmitter::Emit("goal", particlePosition_, 1);
	IIEngine::ParticleEmitter::Emit("petalGroup", petalPosition_, 1);


#ifdef USE_IMGUI

	// 透明度の更新

	ImGui::Begin("ClearScene");


	ImGui::SliderFloat3("cameraPosition", &cameraPosition_.x, -80.0f, 20.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate_.x, -3.14f, 3.14f);

	ImGui::SliderFloat3("particlePosition", &petalPosition_.x, -50.0f, 50.0f);

	ImGui::End();

#endif // USE_IMGUI


	if (IIEngine::Input::GetInstance()->TriggerKey(DIK_RETURN))
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

	if (IIEngine::Input::GetInstance()->TriggerKey(DIK_R))
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

void ClearScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	// プレイヤー描画
	pPlayer_->Draw();

	// フィールド描画
	pField_->Draw();

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
