#pragma once

#include "IPlayerMotion.h"

#include <MyMath.h>

class Player;

class PlayerDeathMotion final : public IPlayerMotion
{
public:
    void Start(Player& pPlayer) override;
    bool Update(Player& pPlayer) override;

    bool IsActive() const override { return motion_.isActive; }
    bool IsComplete() const override { return motion_.isComplete; }
    void Reset() override;

private:

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