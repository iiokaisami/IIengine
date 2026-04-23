#include "PlayerAutoMotion.h"

#include "../Player.h"
#include "TimeManager.h"

using namespace IIEngine;

void PlayerAutoMotion::Reset()
{
    *this = PlayerAutoMotion{};
}

void PlayerAutoMotion::Start(Player& pPlayer) 
{
    active_ = true;
    moveTimer_ = 0.0f;
    attackCooldown_ = 0.0f;

    pPlayer.SetIsCanMove(false);
}

bool PlayerAutoMotion::Update(Player& pPlayer) 
{
    if (!active_) return false;

    const float dt = TimeManager::Instance().GetUnscaledDeltaTime();

    // Auto Move
    if (moveTimer_ <= 0.0f) 
    {
        float randX = (float(rand()) / RAND_MAX) * 2.0f - 1.0f;
        float randZ = (float(rand()) / RAND_MAX) * 2.0f - 1.0f;

        Vector3 dir = { randX, 0.0f, randZ };
        if (dir.Length() < 0.1f) dir.z = 1.0f;
        autoDir_ = dir.Normalize();

        moveTimer_ = 1.0f + static_cast<float>(rand() % 150) / 60.0f;
    }
    moveTimer_ -= dt;

    Vector3 nextPos = pPlayer.GetPosition() + Vector3{ autoDir_.x * pPlayer.GetMoveSpeed().x, 0.0f, autoDir_.z * pPlayer.GetMoveSpeed().z };

    bool redirected = false;
    if (nextPos.x < minX_ || nextPos.x > maxX_) { autoDir_.x = -autoDir_.x; redirected = true; }
    if (nextPos.z < minZ_ || nextPos.z > maxZ_) { autoDir_.z = -autoDir_.z; redirected = true; }
    if (redirected) 
    {
        moveTimer_ = 1.0f + static_cast<float>(rand() % 150) / 60.0f;
    }

    Vector3 vel = { autoDir_.x * pPlayer.GetMoveSpeed().x, 0.0f, autoDir_.z * pPlayer.GetMoveSpeed().z };
    pPlayer.SetVelocity(vel);

    if (vel.Length() > 0.0001f)
    {
        pPlayer.RotationUpdate(vel.Normalize());
    }

    pPlayer.SetPosition(pPlayer.GetPosition() + vel * dt);
    pPlayer.SyncObjectTransform();

    if (attackCooldown_ <= 0.0f) 
    {
        pPlayer.FireBulletForward();
        attackCooldown_ = attackIntervalSec_;
    } 
    else
    {
        attackCooldown_ -= dt;
    }

    return false;
}