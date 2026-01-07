#include "Barrie.h"

void Barrie::Initialize()
{
	// 基底クラス初期化
	StaticObject::Initialize("barrie.obj","Barrie");

	// 大きさセット
	scale_ = defaultScale_;
	targetScale_ = defaultScale_;
	object_->SetScale(scale_);
	// ライト設定
	object_->SetLighting(true);


}

void Barrie::Update()
{

	// 基底クラス更新
	StaticObject::Update();

	// バリア破壊処理
	if (isBarrierDestroyed_ && !isExploding_)
	{
		isExploding_ = true;
		explodeCount_ = 0;
	}

	// 破裂演出処理
	if (isExploding_)
	{
		uint32_t phaseLength = explodeMaxCount_ / 2;
		float scale = 1.0f;

		if (explodeCount_ < phaseLength)
		{
			// 膨張
			float t = static_cast<float>(explodeCount_) / phaseLength;
			scale = Lerp(1.0f, 2.5f, t);
		} else if (explodeCount_ < phaseLength * 2) {
			// 消滅
			float t = static_cast<float>(explodeCount_ - phaseLength) / phaseLength;
			scale = Lerp(2.5f, 0.0f, t);
		}

		// スケール適用
		scale_ = defaultScale_ * scale;
		object_->SetScale(scale_);

		// カウンタ更新
		explodeCount_++;

		// 演出終了
		if (explodeCount_ >= explodeMaxCount_)
		{
			scale_ = { 0.0f, 0.0f, 0.0f };
			object_->SetScale(scale_);
			isExploding_ = false;

			// 破壊フラグをリセット
			isBarrierDestroyed_ = false;

			// パーティクル
			ParticleEmitter::Emit("sparkBurst", position_ , 8);

		}

		// 補間後処理
		if ((scale_ - defaultScale_).Length() < 0.01f)
		{
			targetScale_ = defaultScale_;
		}

	}

	if (scale_.x >= 0.1f)
	{
		// パーティクル
		ParticleEmitter::Emit("spark", { position_.x,position_.y + 3.0f,position_.z }, 1);
	}
}

void Barrie::OnCollisionTrigger(const Collider* _other)
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
		targetScale_ = defaultScale_ * 1.5f;
	}
	
}
