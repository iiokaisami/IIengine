#include "TitleScene.h"

#include <cmath>
#include <ModelManager.h>

#include "TimeManager.h"

using namespace IIEngine;

void TitleScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	camera_ = std::make_unique<SceneCamera>();
	camera_->Initialize();

	cameraPosition_ = camera_->GetRotate();
	cameraRotate_ = camera_->GetPosition();
	cameraPosition_.y = 70.0f;
	cameraPosition_.z = -15.0f;
	cameraRotate_.x = 1.2f;

	cameraManager.AddCamera(camera_->GetCamera());
	cameraManager.SetActiveCamera(0);
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(camera_->GetCamera());



	// --- 2Dスプライト ---
	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		auto sprite = std::make_unique<Sprite>();

		if (i == 0)
		{

			sprite->Initialize("player.dds", { 0,0 }, color_, { 0,0 });
		}
		

		sprites_.push_back(std::move(sprite));

	}

}

void TitleScene::Finalize()
{
	
	// Object3dCommon に設定したデフォルトカメラを解除
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(nullptr);

	// SceneCamera
	if (camera_) 
	{
		camera_->Finalize();
		camera_.reset();
	}
}

void TitleScene::Update()
{

	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();


	// カメラの更新(シェイク、追尾、引き)
	camera_->Update(dt);
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);


	for (auto& sprite : sprites_)
	{
		sprite->Update();

	}



#ifdef USE_IMGUI

	// 透明度の更新

	ImGui::Begin("TitleScene");

	ImGui::SliderFloat4("transparent", &color_.x, 0.0f, 1.0f);

	ImGui::SliderFloat3("cameraPosition", &cameraPosition_.x, -70.0f, 20.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate_.x, -3.14f, 3.14f);

	ImGui::End();

#endif // USE_IMGUI

}

void TitleScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}

}
