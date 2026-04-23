#include "PlayerDeathMotion.h"

#include "../Player.h"           
#include "TimeManager.h"
#include "postEffect/PostEffectManager.h"
#include "../../gameEngine/particle/ParticleEmitter.h"

using namespace IIEngine;

void PlayerDeathMotion::Reset() 
{
    motion_ = Motion{};
}

void PlayerDeathMotion::Start(Player& pPlayer) 
{
    if (motion_.isActive) return;

    motion_.isActive = true;
    motion_.isComplete = false;
    motion_.timerSec = 0.0f;

    position_ = pPlayer.GetPosition();
	rotation_ = pPlayer.GetRotation();
	scale_ = pPlayer.GetScale();

    motion_.startPosition = position_;
    motion_.startRotation = rotation_;
    motion_.startScale = scale_;

    // 死亡中に不要なエフェクトをOFF
    PostEffectManager::GetInstance()->GetPassAs<ScanlinePass>("Scanline")->SetActive(false);
    PostEffectManager::GetInstance()->GetPassAs<ChromaticPulsePass>("ChromaticPulse")->SetActive(false);
}

bool PlayerDeathMotion::Update(Player& pPlayer)
{
    if (!motion_.isActive) return false;

    DeadEffect(pPlayer);
    return motion_.isComplete;
}

void PlayerDeathMotion::DeadEffect(Player& pPlayer)
{
    const float dt = TimeManager::Instance().GetDeltaTime();

    // ぷるぷるフェーズ
    if (motion_.timerSec < motion_.shakeSec) 
    {
        float t = motion_.timerSec / motion_.shakeSec;
        t = std::clamp(t, 0.0f, 1.0f);

        float decay = 1.0f - t;

        float angular = motion_.timerSec * motion_.wobbleFreqHz * (2.0f * 3.14159265f);
        float wobble = std::sin(angular) * motion_.wobbleAmplitude * decay;

        float baseS = motion_.startScale.x;
        float s = std::max(0.001f, baseS + wobble);

        scale_.x = s;
        scale_.y = s;
        scale_.z = s;

        position_ = motion_.startPosition;
        position_.y += std::sin(angular) * 0.01f * decay;

        rotation_ = motion_.startRotation;
        rotation_.y += 0.04f * decay;

		pPlayer.SetPosition(position_);
		pPlayer.SetRotation(rotation_);
		pPlayer.SetScale(scale_);
        pPlayer.SyncObjectTransform();

        motion_.timerSec += dt;
        return;
    }

    // 最後はスケール0に
    scale_ = { 0.0f,0.0f,0.0f };
	pPlayer.SetScale(scale_);
    pPlayer.SyncObjectTransform();

    ParticleEmitter::Emit("rupture", position_, 20);

    motion_.isActive = false;
    motion_.isComplete = true;
}