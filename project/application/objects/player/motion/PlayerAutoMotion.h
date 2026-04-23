#pragma once

#include "IPlayerMotion.h"
#include <MyMath.h>

class Player;

class PlayerAutoMotion final : public IPlayerMotion
{
public:

    void Start(Player& pPlayer) override;
    bool Update(Player& pPlayer) override;

    bool IsActive() const override { return active_; }
    bool IsComplete() const override { return false; } // 終了条件なし
    void Reset() override;

    void SetAttackInterval(float sec) { attackIntervalSec_ = sec; }

private:

    bool active_ = false;

    float moveTimer_ = 0.0f;
    Vector3 autoDir_ = { 0.0f, 0.0f, 1.0f };

    float attackCooldown_ = 0.0f;
    float attackIntervalSec_ = 0.8f;

    // 範囲など
    float minX_ = -15.0f, maxX_ = 15.0f;
    float minZ_ = -15.0f, maxZ_ = 15.0f;
};

