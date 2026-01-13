#include "SceneCamera.h"

#include <Camera.h>
#include <CameraManager.h>


SceneCamera::SceneCamera()
	: camera_(std::make_shared<Camera>()),
	  cameraManager_(CameraManager::GetInstance())
{}

SceneCamera::~SceneCamera() = default;

void SceneCamera::Initialize()
{
	// 必ず先頭でカメラを全クリア
    cameraManager_.ClearAllCameras();
 
	// カメラの作成
	cameraManager_.AddCamera(camera_);
    
	// アクティブカメラの設定
	cameraManager_.SetActiveCamera(0);
 }

void SceneCamera::Finalize()
{
	// カメラの削除
	cameraManager_.RemoveCamera(0);
}

void SceneCamera::AddController(std::unique_ptr<ICameraController> controller)
{
	// コントローラーを追加
	controllers_.push_back(std::move(controller));
}

void SceneCamera::Update(float dt)
{
	// 各コントローラーの更新
	for (auto& controller : controllers_)
	{
		if (controller->IsActive())
		{
			controller->Update(dt);
		}
	}

	// カメラの更新
	if (camera_)
	{
		camera_->Update();
	}
}

Vector3 SceneCamera::GetPosition() const
{
	if (camera_)
	{
		return camera_->GetPosition();
	}
	return Vector3{}; // デフォルト
}

Vector3 SceneCamera::GetRotate() const
{
	if (camera_)
	{
		return camera_->GetRotate();
	}
	return Vector3{};
}

void SceneCamera::SetPosition(const Vector3& pos)
{
	if (this->camera_)
	{
		camera_->SetPosition(pos);
	}
}

void SceneCamera::SetRotate(const Vector3& rot)
{
	if (this->camera_)
	{
		camera_->SetRotate(rot);
	}
}