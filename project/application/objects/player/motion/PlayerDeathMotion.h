#pragma once

#include "IPlayerMotion.h"

#include <MyMath.h>

// 前方宣言
class Player;

/// <summary>
/// プレイヤー死亡時の演出モーション
/// </summary>
class PlayerDeathMotion final : public IPlayerMotion
{
public:

	// 演出開始
    void Start(Player& pPlayer) override;
	// 毎フレーム更新。完了したら true を返す
    bool Update(Player& pPlayer) override;
    
	// 状態のゲッター
    bool IsActive() const override { return motion_.isActive; }
	// 完了のゲッター
    bool IsComplete() const override { return motion_.isComplete; }
	// 再利用したい場合に備えて
    void Reset() override;

private:
	// 死亡エフェクト
    void DeadEffect(Player& player);

private:

    // デフォルトフレームレート
    static constexpr float kDefaultFrameRate = 60.0f;

    struct Motion
    {
        bool isActive = false;
        bool isComplete = false;
        float timerSec = 0.0f;
        float shakeSec = 40.0f / kDefaultFrameRate;
        float wobbleAmplitude = 0.10f;
        float wobbleFreqHz = 10.0f * kDefaultFrameRate;

        Vector3 startPosition = { 0.0f, 0.0f, 0.0f };
        Vector3 startRotation = { 0.0f, 0.0f, 0.0f };
        Vector3 startScale = { 1.0f, 1.0f, 1.0f };

        float popScale = 2.0f;
    };

    Vector3 position_{};
    Vector3 rotation_{};
    Vector3 scale_{};

    Motion motion_;
};