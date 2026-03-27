#include "Barrier.h"

#include "TimeManager.h"

using namespace IIEngine;

void Barrier::Initialize()
{
	// 基底クラス初期化
	StaticObject::Initialize("barrie.obj","Barrie");

	// 大きさセット
	scale_ = defaultScale_;
	targetScale_ = defaultScale_;
	SyncObjectTransform();
	// ライト設定
	object_->SetLighting(true);

	// シェイク
	shakeMagnitude_ = 0.1f;
	shakeDuration_ = 1.0f;
	shakeTimeElapsed_ = 0.0f;
	randomDistribution_ = std::uniform_real_distribution<float>(-1.0f, 1.0f);

}

void Barrier::Update()
{
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// 基底クラス更新
	StaticObject::Update();

	// バリア破壊処理
	if (isBarrierDestroyed_ && !isExploding_)
	{
		isExploding_ = true;
		explodeCount_ = 0;
		basePosition_ = GetPosition();
	}

	// 破裂演出処理
	if (isExploding_)
	{
		float scale = 1.0f;

		if (explodeCount_ < shakeMaxCount_)
		{
			// シェイク
			UpdateShake();
		}
		else if (explodeCount_ < shakeMaxCount_ + explodeMaxCount_)
		{
			// 膨張
			float t = static_cast<float>(explodeCount_) / explodeMaxCount_;
			scale = Lerp(1.0f, explodeScalePeak_, t);
		}
		else
		{
			// 消滅
			float t = static_cast<float>(explodeCount_ - explodeMaxCount_) / explodeMaxCount_;
			scale = Lerp(explodeScalePeak_, 0.0f, t);
		}

		// スケール適用
		scale_ = defaultScale_ * scale;
		SyncObjectTransform();

		// カウンタ更新
		explodeCount_ += dt;

		// 演出終了
		if (explodeCount_ >= shakeMaxCount_ + (explodeMaxCount_ * 2))
		{
			scale_ = { 0.0f, 0.0f, 0.0f };
			SyncObjectTransform();
			isExploding_ = false;

			// 破壊フラグをリセット
			isBarrierDestroyed_ = false;

			// 墓移管料フラグをセット
			isBarrierDestroyedComplete_ = true;

			// パーティクル
			IIEngine::ParticleEmitter::Emit("sparkBurst", position_ , explodeParticleCount_);

		}

		// 補間後処理
		if ((scale_ - defaultScale_).Length() < scaleReturnEpsilon_)
		{
			targetScale_ = defaultScale_;
		}

	}

	if (scale_.x >= sparkMinVisibleScaleX_)
	{
		// パーティクル
		IIEngine::ParticleEmitter::Emit("spark", { position_.x,position_.y + sparkEmitHeight_ ,position_.z }, 1);
	}
}

void Barrier::OnCollisionTrigger(const Collider* _other)
{
	if (_other->GetColliderID() == "PlayerBullet" or
		_other->GetColliderID() == "Player" or
		_other->GetColliderID() == "TrapEnemy" or
		_other->GetColliderID() == "NormalEnemy" or
		_other->GetColliderID() == "EnemyBullet" or
		_other->GetColliderID() == "VignetteTrap" or
		_other->GetColliderID() == "SetTimeBomb")
	{
		// 一瞬大きくし、元に戻す
		targetScale_ = defaultScale_ * hitScaleMultiplier_;
	}
}

void Barrier ::UpdateShake()
{
	float offsetX = randomDistribution_(randomEngine_) * shakeMagnitude_;
	float offsetZ = randomDistribution_(randomEngine_) * shakeMagnitude_;

	// カメラの位置をシェイク
	SetPosition(basePosition_ + Vector3(offsetX, 0.0f, offsetZ));

}