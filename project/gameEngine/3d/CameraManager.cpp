#include "CameraManager.h"

namespace IIEngine
{

    void CameraManager::AddCamera(std::shared_ptr<Camera> camera)
    {
        cameras_.push_back(camera);
        if (activeCameraIndex_ == std::numeric_limits<uint32_t>::max())
        {
            activeCameraIndex_ = 0;  // 最初のカメラをアクティブにする
        }
    }

    void CameraManager::RemoveCamera(uint32_t index)
    {
        if (index < cameras_.size())
        {
            cameras_.erase(cameras_.begin() + index);
            if (index == activeCameraIndex_)
            {
                activeCameraIndex_ = cameras_.empty() ? std::numeric_limits<uint32_t>::max() : 0;
            } else if (index < activeCameraIndex_)
            {
                --activeCameraIndex_;
            }
        }
    }

    void CameraManager::SetActiveCamera(uint32_t index)
    {
        if (index < cameras_.size())
        {
            activeCameraIndex_ = index;
        }
    }

    std::shared_ptr<Camera> CameraManager::GetActiveCamera() const
    {
        if (activeCameraIndex_ < cameras_.size())
        {
            return cameras_[activeCameraIndex_];
        }
        return nullptr;
    }

    void CameraManager::UpdateAll(float deltaTime)
    {
        for (auto& camera : cameras_)
        {
            if (deltaTime > 0.0f)
            {
                camera->UpdateShake(deltaTime);
            }
            camera->Update();
        }
    }

    void CameraManager::ShakeSpecificCamera(uint32_t index, float duration, float magnitude)
    {
        if (index < cameras_.size())
        {
            cameras_[index]->StartShake(duration, magnitude);
        }
    }

    void CameraManager::StartShakeActiveCamera(float duration, float magnitude)
    {
        ShakeSpecificCamera(activeCameraIndex_, duration, magnitude);
    }

    void CameraManager::ClearAllCameras()
    {
        cameras_.clear();
        activeCameraIndex_ = std::numeric_limits<uint32_t>::max();
    }
}