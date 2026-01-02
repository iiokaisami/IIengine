#include "StartCameraController.h"

#include <algorithm>

#include <Camera.h>
#include <Ease.h>
#include <MyMath.h>

StartCameraController::StartCameraController(std::shared_ptr<Camera> camera,
    const Vector3& cameraStart, const Vector3& cameraControl1, const Vector3& cameraControl2, const Vector3& cameraEnd,
    const Vector3& cameraStartRot, const Vector3& cameraEndRot, float duration)
    : camera_(std::move(camera)),
      startPos_(cameraStart),
      control1_(cameraControl1),
      control2_(cameraControl2),
      endPos_(cameraEnd),
      startRot_(cameraStartRot),
      endRot_(cameraEndRot),
      duration_(duration)
{
}

void StartCameraController::Update(float dt)
{
	// 非アクティブまたはカメラが無効なら何もしない
    if (!active_ or !camera_)
    {
        return;
    }

    timer_ += dt;
    float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);
    float t_eased = Ease::InOutQuad(t);

    // 位置
    Vector3 camPos = Bezier3(startPos_, control1_, control2_, endPos_, t_eased);
    // 回転
    Vector3 camRot = Lerp(startRot_, endRot_, t_eased);

    camera_->SetPosition(camPos);
    camera_->SetRotate(camRot);

    if (t >= 1.0f)
    {
        // 演出終了
        camera_->SetPosition(endPos_);
        camera_->SetRotate(endRot_);
        active_ = false;

		// コールバック呼び出し
        if (onFinish_)
        {
            onFinish_();
		}
    }
}

void StartCameraController::Start()
{
    if (!camera_) return;
    timer_ = 0.0f;
    active_ = true;
    // 初期位置を即セットしておく（任意）
    camera_->SetPosition(startPos_);
    camera_->SetRotate(startRot_);
}

void StartCameraController::Stop()
{
    active_ = false;
}