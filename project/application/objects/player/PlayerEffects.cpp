#include "PlayerEffects.h"

#include "Player.h"

#include "TimeManager.h"
#include "postEffect/PostEffectManager.h"

using namespace IIEngine;

void PlayerEffects::Initialize()
{
    Reset();
}

void PlayerEffects::Reset()
{
    // Slow
    slowActive_ = false;
    slowTimerSec_ = 0.0f;
    radialStrength_ = 0.0f;

    // RGBShift
    rgbShiftActive_ = false;
    rgbShiftTimer_ = 0.0f;

    // Noise
    noiseEnabled_ = true;
    touchGoal_ = false;
    finalNoiseStrength_ = 0.0f;

    // Vignette
    vignetteHit_ = false;
    vignetteFadingOut_ = false;
    vignetteStrength_ = 0.0f;
    vignetteRemainingSec_ = kMaxVignetteSec_;

    // 初期状態を安全に
    TimeManager::Instance().SetTimeScale(1.0f);

    // PostEffect 初期値を安全に（必要に応じて）
    if (auto* pe = PostEffectManager::GetInstance())
    {
        // RadialBlur
        pe->SetActiveEffect("RadialBlur", false);
        if (auto* radial = pe->GetPassAs<RadialBlurPass>("RadialBlur"))
        {
            radial->SetStrength(0.0f);
        }

        // RGBShift は Initialize 側で有効化しているならここでは触らない選択もOK
        if (auto* rgb = pe->GetPassAs<RGBShiftPass>("RGBShift"))
        {
            rgb->SetIntensity(0.0f);
        }

        // Noise
        pe->SetActiveEffect("Noise", false);
        if (auto* noise = pe->GetPassAs<NoisePass>("Noise"))
        {
            noise->SetIntensity(0.0f);
        }

        // Vignette
        pe->SetActiveEffect("Vignette", false);
        if (auto* vig = pe->GetPassAs<VignettePass>("Vignette"))
        {
            vig->SetStrength(0.0f);
        }
    }
}

void PlayerEffects::OnDamaged()
{
	// 被弾エフェクト開始
    rgbShiftActive_ = true;
    rgbShiftTimer_ = 0.0f;
}

void PlayerEffects::OnDead()
{}

void PlayerEffects::OnTouchGoal(bool touched)
{
	// ゴールに触れた/離れた状態を更新
    touchGoal_ = touched;
}

void PlayerEffects::OnHitVignetteTrap()
{
	// 暗闇トラップにヒットした
    vignetteHit_ = true;
}

void PlayerEffects::StartEvadeSlow()
{
	// 回避スロー開始
    slowActive_ = true;
    slowTimerSec_ = kSlowHoldTime_ + kSlowRecoverTime_;

	// RadialBlur を有効化
    if (auto* pe = PostEffectManager::GetInstance())
    {
        pe->SetActiveEffect("RadialBlur", true);
    }
}

void PlayerEffects::Update(Player& player)
{
	// 毎フレーム更新
    UpdateEvadeSlow();
    UpdateDamageRGBShift();
    UpdateVignetteTrap(player);
    UpdateHPDecreaseNoise(player);
}

void PlayerEffects::UpdateEvadeSlow()
{
    if (!slowActive_)
    {
        return;
    }

	// タイマー更新
    slowTimerSec_ -= TimeManager::Instance().GetUnscaledDeltaTime();

	// タイムスケールと RadialBlur の強さを更新
    if (slowTimerSec_ > kSlowRecoverTime_)
    {
        // 固定スロー区間
        TimeManager::Instance().SetTimeScale(0.1f);
        radialStrength_ = 0.4f;
    }
    else
    {
        // 復帰区間
        float t = 1.0f - (slowTimerSec_ / kSlowRecoverTime_);
        t = std::clamp(t, 0.0f, 1.0f);

        float timeScale = std::lerp(0.1f, 1.0f, t);
        TimeManager::Instance().SetTimeScale(timeScale);

        radialStrength_ = std::lerp(0.8f, 0.0f, t);
    }

	// RadialBlur の強さを更新
    if (auto* pe = PostEffectManager::GetInstance())
    {
        if (auto* radial = pe->GetPassAs<RadialBlurPass>("RadialBlur"))
        {
            radial->SetStrength(radialStrength_);
        }
    }

	// タイマー終了チェック
    if (slowTimerSec_ <= 0.0f)
    {
        slowActive_ = false;
        slowTimerSec_ = 0.0f;

        TimeManager::Instance().SetTimeScale(1.0f);

		// RadialBlur を無効化
        if (auto* pe = PostEffectManager::GetInstance())
        {
			// 念のため強さもリセット
            if (auto* radial = pe->GetPassAs<RadialBlurPass>("RadialBlur"))
            {
                radial->SetStrength(0.0f);
            }
            pe->SetActiveEffect("RadialBlur", false);
        }
    }
}

void PlayerEffects::UpdateDamageRGBShift()
{
    if (!rgbShiftActive_)
    {
        return;
    }

	// タイマー更新
    const float dt = TimeManager::Instance().GetDeltaTime();
    rgbShiftTimer_ += dt;

	// 強さ計算
    if (rgbShiftTimer_ >= rgbShiftDuration_)
    {
        rgbShiftActive_ = false;
        rgbShiftTimer_ = 0.0f;

        if (auto* pe = PostEffectManager::GetInstance())
        {
            if (auto* rgb = pe->GetPassAs<RGBShiftPass>("RGBShift"))
            {
                rgb->SetIntensity(0.0f);
            }
        }
        return;
    }

	// 時間経過に伴う減衰と振動を組み合わせた強さ計算
    float t = rgbShiftTimer_;
    float power = 0.0f;

	// 振動は時間経過とともに減衰するように、指数関数的な減衰と正弦波の振動を組み合わせる
    power = rgbShiftAmplitude_ * std::abs(std::sin(t * rgbShiftOscillation_)) * std::exp(-t * rgbShiftDamping_);

	// RGBShift の強さを更新
    if (auto* pe = PostEffectManager::GetInstance())
    {
        if (auto* rgb = pe->GetPassAs<RGBShiftPass>("RGBShift"))
        {
            rgb->SetIntensity(power);
        }
    }
}

void PlayerEffects::UpdateHPDecreaseNoise(Player& player)
{
    if (!noiseEnabled_)
    {
		// Noise 効果が無効化されている場合は何もしない
        if (auto* pe = PostEffectManager::GetInstance())
        {
            pe->SetActiveEffect("Noise", false);
        }
        return;
    }

	// プレイヤーが死亡している場合はノイズを徐々に減衰させる
    if (player.IsDead())
    {
        // 死亡中はノイズを落とす
        finalNoiseStrength_ = Lerp(finalNoiseStrength_, 0.0f, 0.02f);

        if (auto* pe = PostEffectManager::GetInstance())
        {
            if (auto* noise = pe->GetPassAs<NoisePass>("Noise"))
            {
                noise->SetIntensity(finalNoiseStrength_);
            }
        }
        return;
    }

	// オート制御中はノイズを無効化
    if (player.IsAutoControl())
    {
        if (auto* pe = PostEffectManager::GetInstance())
        {
            pe->SetActiveEffect("Noise", false);
        }
        return;
    }

    const float hp = player.GetHP();

    const float maxHP = player.GetMaxHP();

    float hpRate = 1.0f;
    if (maxHP > 0.0f)
    {
        hpRate = hp / maxHP;
    }

	// HPがこの値を下回るとノイズが発生し始める
    const float dangerThreshold = hpDangerThreshold_;

    auto* pe = PostEffectManager::GetInstance();
    if (!pe) return;

    auto* noise = pe->GetPassAs<NoisePass>("Noise");
    if (!noise) return;

	// ゴールに触れている場合はノイズを徐々に減衰させる
    if (touchGoal_)
    {
        finalNoiseStrength_ = Lerp(finalNoiseStrength_, 0.0f, 0.02f);
        noise->SetIntensity(finalNoiseStrength_);
        return;
    }

	// HPが危険域を下回っている場合はノイズを強くする
    if (hpRate < dangerThreshold)
    {
        float t = 1.0f - (hpRate / dangerThreshold);
        t = std::clamp(t, 0.0f, 1.0f);

        float eased = std::pow(t, hpNoisePow_);

        float time = TimeManager::Instance().GetTotalTime();
        float pulse = std::sin(time * hpNoisePulseFreq_) * hpNoisePulseAmp_;

        finalNoiseStrength_ = eased * hpNoiseBaseGain_ + pulse;
        finalNoiseStrength_ = std::clamp(finalNoiseStrength_, 0.0f, 1.0f);

        pe->SetActiveEffect("Noise", true);
        noise->SetIntensity(finalNoiseStrength_);
    } 
    else
    {
        pe->SetActiveEffect("Noise", false);
    }
}

void PlayerEffects::UpdateVignetteTrap(Player& player)
{
    const float dt = TimeManager::Instance().GetDeltaTime();

    auto* pe = PostEffectManager::GetInstance();
    if (!pe) return;

    auto* vig = pe->GetPassAs<VignettePass>("Vignette");
    if (!vig) return;

    // フェードアウト中
    if (vignetteFadingOut_)
    {
        static const float fadeDurationSec = 30.0f / kDefaultFrameRate_;
        static float fadeTimer = 0.0f;

        fadeTimer += dt;

        float t = fadeTimer / fadeDurationSec;
        t = std::clamp(t, 0.0f, 1.0f);

        vignetteStrength_ = std::lerp(1.8f, 0.0f, t);
        vig->SetStrength(vignetteStrength_);

        if (t >= 1.0f)
        {
            vignetteFadingOut_ = false;
            fadeTimer = 0.0f;
            vignetteStrength_ = 0.0f;

            pe->SetActiveEffect("Vignette", vignetteHit_);

        }
        return;
    }

    // 通常の効果中
    if (vignetteHit_)
    {
        const float elapsedSec = kMaxVignetteSec_ - vignetteRemainingSec_;
        const float fadeInDurationSec = 30.0f / kDefaultFrameRate_;

        if (elapsedSec < fadeInDurationSec)
        {
            float t = elapsedSec / fadeInDurationSec;
            t = std::clamp(t, 0.0f, 1.0f);
            vignetteStrength_ = std::lerp(0.0f, 1.8f, t);
        } else
        {
            vignetteStrength_ = 1.8f;
        }

        pe->SetActiveEffect("Vignette", vignetteHit_);
        vig->SetStrength(vignetteStrength_);

        ParticleEmitter::Emit("debuff", player.GetPosition(), 2);

        vignetteRemainingSec_ -= dt;
        if (vignetteRemainingSec_ <= 0.0f)
        {
            vignetteHit_ = false;
            vignetteFadingOut_ = true;
            vignetteRemainingSec_ = kMaxVignetteSec_;
        }
    }
}