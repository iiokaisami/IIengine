#pragma once

#include "IPlayerMotion.h"

#include <MyMath.h>

// 前方宣言
class Player;

/// <summary>
/// ゲームクリア時の演出モーション 
/// </summary>
class PlayerClearMotion final : public IPlayerMotion 
{
public:

	// クリアモーション開始
    void Start(Player& pPlayer) override;
	// クリアモーション更新
    bool Update(Player& pPlayer) override;

	// クリアモーションがアクティブか
    bool IsActive() const override { return active_; }
	// クリアモーションが完了したか
    bool IsComplete() const override { return complete_; }
	// クリアモーションの状態をリセット
    void Reset() override;

private:
    
	// クリアモーションの状態
    bool active_ = false;
    bool complete_ = false;

	// モーションパラメーター
    Vector3 center_ = { 0.0f, 0.5f, 0.0f };
    float radius_ = 8.0f;
    float angularSpeed_ = 1.5f;
    float currentAngle_ = 0.2f;
    bool clockwise_ = false;

    float poyoAmpX_ = 0.2f;
    float poyoAmpZ_ = 0.2f;
    float poyoAmpY_ = 0.2f;
    float poyoFreqXZ_ = 1.2f;
    float poyoFreqY_ = 1.2f;
    float poyoPhase_ = 0.0f;
    float poyoPhaseY_ = 0.0f;
    bool maintainArea_ = false;
    bool maintainVolume_ = false;
    Vector3 baseScale_ = { 1.0f, 1.0f, 1.0f };

    float totalTime_ = 0.0f;
    float lastYaw_ = 0.0f;
};