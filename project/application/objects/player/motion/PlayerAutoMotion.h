#pragma once

#include "IPlayerMotion.h"
#include <MyMath.h>

// 前方宣言
class Player;

/// <summary>
/// プレイヤーの自動モーション
/// </summary>
class PlayerAutoMotion final : public IPlayerMotion
{
public:

	// ランダムに動き回るモーション
    void Start(Player& pPlayer) override;
    
	// 移動と攻撃を行う。終了条件なしで、外部から止める必要がある
    bool Update(Player& pPlayer) override;
	// 常にアクティブ
    bool IsActive() const override { return active_; }
	// 終了条件なし
    bool IsComplete() const override { return false; } 
	// 再利用のためのリセット
    void Reset() override;
	// 攻撃間隔のセッター
    void SetAttackInterval(float sec) { attackIntervalSec_ = sec; }

private:

	// アクティブフラグ
    bool active_ = false;

	// 移動関連
    float moveTimer_ = 0.0f;
    Vector3 autoDir_ = { 0.0f, 0.0f, 1.0f };

    float attackCooldown_ = 0.0f;
    float attackIntervalSec_ = 0.8f;

    // 範囲など
    float minX_ = -15.0f, maxX_ = 15.0f;
    float minZ_ = -15.0f, maxZ_ = 15.0f;
};

