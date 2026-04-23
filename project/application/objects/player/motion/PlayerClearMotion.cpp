#include "PlayerClearMotion.h"

#include "../Player.h"
#include "TimeManager.h"
#include "../../gameEngine/particle/ParticleEmitter.h"

using namespace IIEngine;

void PlayerClearMotion::Reset() 
{
    *this = PlayerClearMotion{};
}

void PlayerClearMotion::Start(Player& pPlayer)
{
    active_ = true;
    complete_ = false;
    totalTime_ = 0.0f;

    // クリア中は移動不可にするならここで
    pPlayer.SetIsCanMove(false);
}

bool PlayerClearMotion::Update(Player& pPlayer) 
{
    if (!active_) return false;

    const float dt = TimeManager::Instance().GetUnscaledDeltaTime();
    totalTime_ += dt;

    constexpr float TWO_PI = 3.14159265358979323846f * 2.0f;

    float dir = clockwise_ ? -1.0f : 1.0f;
    currentAngle_ += dir * angularSpeed_ * dt;

    if (currentAngle_ > TWO_PI) currentAngle_ = std::fmod(currentAngle_, TWO_PI);
    else if (currentAngle_ < 0.0f) currentAngle_ = std::fmod(currentAngle_ + TWO_PI, TWO_PI);

    // 位置
    Vector3 pos;
    pos.x = center_.x + radius_ * std::cos(currentAngle_);
    pos.y = center_.y;
    pos.z = center_.z + radius_ * std::sin(currentAngle_);

    // 向き
    float velX = -radius_ * std::sin(currentAngle_) * (dir * angularSpeed_);
    float velZ = radius_ * std::cos(currentAngle_) * (dir * angularSpeed_);
    if (!(std::abs(velX) < 1e-6f && std::abs(velZ) < 1e-6f)) {
        lastYaw_ = std::atan2(velX, velZ);
    }

    Vector3 rot{ 0.0f, lastYaw_, 0.0f };

    // スケール
    float omegaXZ = TWO_PI * poyoFreqXZ_;
    float sXZ = std::sin(omegaXZ * totalTime_ + poyoPhase_);
    float sx = std::max(0.05f, 1.0f + poyoAmpX_ * sXZ);

    float sz;
    if (maintainArea_)
    {
        sz = std::clamp(1.0f / sx, 0.05f, 5.0f);
    }
    else
    {
        sz = std::clamp(1.0f + poyoAmpZ_ * std::sin(omegaXZ * totalTime_ + poyoPhase_ + 3.14159265f / 2.0f), 0.05f, 5.0f);
    }

    float sy;
    if (maintainVolume_) 
    {
        float sx_rel = std::max(0.01f, sx);
        float sz_rel = std::max(0.01f, sz);
        float sy_rel = std::clamp(1.0f / (sx_rel * sz_rel), 0.05f, 5.0f);
        sy = baseScale_.y * sy_rel;
    } 
    else
    {
        float omegaY = TWO_PI * poyoFreqY_;
        float sY = std::sin(omegaY * totalTime_ + poyoPhaseY_);
        float sy_rel = std::clamp(1.0f + poyoAmpY_ * sY, 0.05f, 5.0f);
        sy = baseScale_.y * sy_rel;
    }

    Vector3 scale;
    scale.x = baseScale_.x * sx;
    scale.y = sy;
    scale.z = baseScale_.z * sz;

    pPlayer.SetPosition(pos);
    pPlayer.SetRotation(rot);
    pPlayer.SetScale(scale);
    pPlayer.SyncObjectTransform();

    ParticleEmitter::Emit("walk", pos, 1);

    // ClearScene専用で終わりがないなら false のまま
    return complete_;
}