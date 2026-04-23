#pragma once

#include <MyMath.h>

class Player;

namespace IIEngine
{
    class TextureManager;
}

/// <summary>
/// Player専用：ポストエフェクト/スロー/トラップ等の演出管理
/// - RGBShift
/// - Noise
/// - VignetteTrap
/// - Evade slow-motion
/// </summary>
class PlayerEffects
{
public:

    PlayerEffects() = default;
    ~PlayerEffects() = default;

	// 初期化
    void Initialize();

    // リセット
    void Reset();

    // 毎フレーム更新
    void Update(Player& pPlayer);

    // 被弾した
    void OnDamaged();

    // ゴール接触状態が変わった
    void OnTouchGoal(bool touched);

    // 死亡した
    void OnDead();

    // ゴールに触れた/離れた
    void SetTouchGoal(bool touch);

    // VignetteTrap の有効化
    void OnHitVignetteTrap();

    // 回避スロー開始
    void StartEvadeSlow();

    // 外から強制的に Noise の有効/無効を切りたい場合
    void SetNoiseEnabled(bool enabled) { noiseEnabled_ = enabled; }

private:

    // ---- 内部更新 ----
    void UpdateEvadeSlow();
    void UpdateDamageRGBShift();
    void UpdateHPDecreaseNoise(Player& pPlayer);
    void UpdateVignetteTrap(Player& pPlayer);

private:

	// デフォルトのフレームレート
	static constexpr float kDefaultFrameRate_ = 60.0f;

    // ==== Evade slow ====
    bool  slowActive_ = false;
    float slowTimerSec_ = 0.0f;

    // 元Player定数
    float kSlowHoldTime_ = 0.18f;
    float kSlowRecoverTime_ = 0.15f;

    float radialStrength_ = 0.0f;

    // ==== RGBShift ====
    bool  rgbShiftActive_ = false;
    float rgbShiftTimer_ = 0.0f;

    float rgbShiftDuration_ = 0.35f;
    float rgbShiftAmplitude_ = 0.25f;
    float rgbShiftOscillation_ = 40.0f;
    float rgbShiftDamping_ = 8.0f;

    // ==== Noise ====
    bool noiseEnabled_ = true;
    bool touchGoal_ = false; // OnTouchGoalで更新

    float finalNoiseStrength_ = 0.0f;

    float hpDangerThreshold_ = 0.3f;
    float hpNoisePow_ = 2.5f;
    float hpNoisePulseFreq_ = 12.0f;
    float hpNoisePulseAmp_ = 0.15f;
    float hpNoiseBaseGain_ = 2.5f;

    // ==== VignetteTrap ====
    bool  vignetteHit_ = false;
    bool  vignetteFadingOut_ = false;

    float vignetteStrength_ = 0.0f;

    // 最大時間
    static constexpr float kMaxVignetteSecConst_ = (60.0f * 3.0f) / kDefaultFrameRate_;

    float kMaxVignetteSec_ = kMaxVignetteSecConst_;
    float vignetteRemainingSec_ = kMaxVignetteSecConst_;

};