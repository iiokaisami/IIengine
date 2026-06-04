#include "DeathCameraController.h"

#include <Camera.h>
#include <Ease.h>

DeathCameraController::DeathCameraController(std::shared_ptr<IIEngine::Camera> camera, const std::function<Vector3()>& targetPos, float duration, float rotations, float endRadius, float endHeight)
	: camera_(std::move(camera)),
      getPlayerPos_(std::move(targetPos)),
      duration_(duration),
      rotations_(rotations),
      endRadius_(endRadius),
	  endHeight_(endHeight)
{
}


void DeathCameraController::Update(float dt)
{
    // 非アクティブまたはカメラが無効なら何もしない
    if (!active_ or !camera_ or !getPlayerPos_)
    {
        return;
    }

    // 経過時間を進める
    timer_ += dt;
    float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);
    float t_eased = Ease::InOutQuad(t);

    float targetAngle = 0.0f; // 最終的にプレイヤー前方が 0
    float delta = targetAngle - startAngle_;
    // normalize delta to [-pi, pi]
    while (delta > (float)M_PI) delta -= kTau_;
    while (delta < (float)-M_PI) delta += kTau_;

    float totalAngularTravel = delta + rotations_ * kTau_;
    float angle = startAngle_ + totalAngularTravel * t_eased;

    // 半径・高さを補間
    float radius = std::lerp(startRadius_, endRadius_, t_eased);
    float height = std::lerp(startHeight_, endHeight_, t_eased);

    // カメラ位置を設定
    Vector3 playerPos = getPlayerPos_();
    Vector3 camPos;
    camPos.x = playerPos.x + std::sin(angle) * radius;
    camPos.z = playerPos.z + std::cos(angle) * radius;
    camPos.y = playerPos.y + height;

    camera_->SetPosition(camPos);

    // カメラ回転をプレイヤー方向に向ける
    Vector3 dir = (playerPos - camPos);
    float yaw = std::atan2(dir.x, dir.z);
    float horizontalDist = std::sqrt(dir.x * dir.x + dir.z * dir.z);
    float pitch = std::atan2(-dir.y, horizontalDist);

    camera_->SetRotate({ pitch, yaw, 0.0f });

    if (t >= 1.0f)
    {
        active_ = false;

        // 最終位置を厳密にセット
        Vector3 finalPos;
        finalPos.x = playerPos.x + std::sin(targetAngle) * endRadius_;
        finalPos.z = playerPos.z + std::cos(targetAngle) * endRadius_;
        finalPos.y = playerPos.y + endHeight_;
        camera_->SetPosition(finalPos);

        Vector3 finalDir = (playerPos - finalPos);
        float finalYaw = std::atan2(finalDir.x, finalDir.z);
        float finalH = std::sqrt(finalDir.x * finalDir.x + finalDir.z * finalDir.z);
        float finalPitch = std::atan2(-finalDir.y, finalH);
        camera_->SetRotate({ finalPitch, finalYaw, 0.0f });

        // 完了コールバック
        if (onFinish_)
        {
            onFinish_();
        }
    }
}

void DeathCameraController::Start()
{
	// カメラまたはプレイヤー位置関数が無効なら何もしない
    if (!camera_ or !getPlayerPos_)
    {
        return;
    }

    timer_ = 0.0f;
    active_ = true;

    // プレイヤー位置と現在カメラ位置から開始角度・半径・高さを算出
    Vector3 playerPos = getPlayerPos_();
    Vector3 camPos = camera_->GetPosition();

    float dx = camPos.x - playerPos.x;
    float dz = camPos.z - playerPos.z;

    // startAngle を atan2(dx, dz)の順で取る
    startAngle_ = std::atan2(dx, dz);
    startRadius_ = std::sqrt(dx * dx + dz * dz);
    startHeight_ = camPos.y - playerPos.y;
}

void DeathCameraController::Stop()
{
    active_ = false;
}