#include "ClearCameraController.h"

#include <algorithm>
#include <cmath>
#include <corecrt_math_defines.h>

#include <Camera.h>
#include <Ease.h>

ClearCameraController::ClearCameraController(std::shared_ptr<Camera> camera, std::function<Vector3()> targetPos, float duration, float rotations, float rotationSpeedMultiplier, float heightRise, float fadeStartThreshold)
    : camera_(std::move(camera)),
      getPlayerPos_(std::move(targetPos)),
      duration_(duration), 
      rotations_(rotations),
      rotationSpeedMultiplier_(rotationSpeedMultiplier),
      heightRise_(heightRise),
      fadeStartThreshold_(fadeStartThreshold)
{
}

void ClearCameraController::Start()
{
    if (!camera_ or !getPlayerPos_)
    {
        return;
    }

    timer_ = 0.0f;
    fadeSignaled_ = false;
    active_ = true;

    // 開始時のプレイヤー位置とカメラ位置から start angle/radius/height を計算
    Vector3 playerPos = getPlayerPos_();
    Vector3 camPos = camera_->GetPosition();

    float dx = camPos.x - playerPos.x;
    float dz = camPos.z - playerPos.z;

    // startAngle を atan2(dx, dz) の順で取る（既存コードと整合）
    startAngle_ = std::atan2(dx, dz);
    startRadius_ = std::sqrt(dx * dx + dz * dz);
    startHeight_ = camPos.y - playerPos.y;
}

void ClearCameraController::Stop()
{
    active_ = false;
}

void ClearCameraController::Update(float dt)
{
    if (!active_ or !camera_ or !getPlayerPos_)
    {
        return;
    }

    timer_ += dt;
    float t = std::clamp(timer_ / duration_, 0.0f, 1.0f);

    float t_eased = Ease::InOutQuad(t);

    // 角速度抑制の倍率を適用
    float rotationsScaled = rotations_ * rotationSpeedMultiplier_;
    float targetAngle = 0.0f; // プレイヤー正面
    float delta = targetAngle - startAngle_;
    while (delta > (float)M_PI) delta -= 2.0f * (float)M_PI;
    while (delta < (float)-M_PI) delta += 2.0f * (float)M_PI;

    float totalAngularTravel = delta + rotationsScaled * 2.0f * (float)M_PI;
    float angle = startAngle_ + totalAngularTravel * t_eased;

    // 半径は開始時のものを維持（元コード）
    const float radius = startRadius_;

    // 高さは開始高さから徐々に上昇
    float height = startHeight_ + heightRise_ * t_eased;

    // カメラ位置（極座標から）
    Vector3 playerPos = getPlayerPos_();
    Vector3 camPos;
    camPos.x = playerPos.x + std::sin(angle) * radius;
    camPos.z = playerPos.z + std::cos(angle) * radius;
    camPos.y = playerPos.y + height;

    camera_->SetPosition(camPos);

    // カメラ回転：プレイヤーを見る方向
    Vector3 dir = (playerPos - camPos);
    float yaw = std::atan2(dir.x, dir.z);
    float horizontalDist = std::sqrt(dir.x * dir.x + dir.z * dir.z);
    float pitch = std::atan2(-dir.y, horizontalDist);

    camera_->SetRotate({ pitch, yaw, 0.0f });

    // フェード開始判定
    if (!fadeSignaled_ && t >= fadeStartThreshold_)
    {
        fadeSignaled_ = true;
        if (onFadeStart_) onFadeStart_();
    }

    // 終了判定
    if (t >= 1.0f)
    {
        active_ = false;
        // 最終位置を厳密セット
        Vector3 finalPos;
        finalPos.x = playerPos.x + std::sin(targetAngle) * radius;
        finalPos.z = playerPos.z + std::cos(targetAngle) * radius;
        finalPos.y = playerPos.y + (startHeight_ + heightRise_);

        camera_->SetPosition(finalPos);

        Vector3 finalDir = (playerPos - finalPos);
        float finalYaw = std::atan2(finalDir.x, finalDir.z);
        float finalH = std::sqrt(finalDir.x * finalDir.x + finalDir.z * finalDir.z);
        float finalPitch = std::atan2(-finalDir.y, finalH);
        camera_->SetRotate({ finalPitch, finalYaw, 0.0f });

        if (onFinish_)
        {
            onFinish_();
        }
    }
}