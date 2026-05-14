#include "PlayerAutoMotion.h"

#include "../Player.h"
#include "TimeManager.h"

using namespace IIEngine;

void PlayerAutoMotion::Reset()
{
	// 再利用のために状態をリセット
    *this = PlayerAutoMotion{};
}

void PlayerAutoMotion::Start(Player& pPlayer) 
{
	// すでにアクティブなら開始しない
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

	// 次の位置を予測して、範囲外なら方向を反転
    Vector3 nextPos = pPlayer.GetPosition() + Vector3{ autoDir_.x * pPlayer.GetMoveSpeed().x, 0.0f, autoDir_.z * pPlayer.GetMoveSpeed().z };

	// 範囲外に出るなら方向を反転して、リダイレクトしたら次の移動までの時間を少し増やす
    bool redirected = false;
    if (nextPos.x < minX_ || nextPos.x > maxX_) { autoDir_.x = -autoDir_.x; redirected = true; }
    if (nextPos.z < minZ_ || nextPos.z > maxZ_) { autoDir_.z = -autoDir_.z; redirected = true; }
    if (redirected) 
    {
        moveTimer_ = 1.0f + static_cast<float>(rand() % 150) / 60.0f;
    }

	// 移動と回転
    Vector3 vel = { autoDir_.x * pPlayer.GetMoveSpeed().x, 0.0f, autoDir_.z * pPlayer.GetMoveSpeed().z };
    pPlayer.SetVelocity(vel);

	// 向き更新。移動速度がほぼ0なら向きは変えない
    if (vel.Length() > 0.0001f)
    {
        pPlayer.RotationUpdate(vel.Normalize());
    }

	// 位置更新
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