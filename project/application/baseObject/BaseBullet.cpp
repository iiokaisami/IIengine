#include "BaseBullet.h"

#include "TimeManager.h"

void BaseBullet::Finalize()
{
	// コライダー削除
	DeleteColliderSafe();
}

void BaseBullet::Update()
{
	// 寿命管理
	UpdateLifetime();

	// モデル更新
	if (object_)
	{
		SyncObjectTransform();
	}
}

void BaseBullet::Draw()
{
	if (object_)
	{
		object_->Draw();
	}
}

void BaseBullet::SetVelocity(const Vector3& _velocity)
{
	// フレームレート補正
	velocity_ = _velocity * kDefaultFrameRate;
}

void BaseBullet::UpdateLifetime()
{
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();
	if (deathRemainingSeconds_ > 0.0f)
	{
		deathRemainingSeconds_ -= dt;
		if (deathRemainingSeconds_ <= 0.0f)
		{
			// 寿命切れ
			SetIsDead(true);
		}
	}
}

void BaseBullet::DeleteColliderSafe()
{
	if (colliderManager_)
	{
		colliderManager_->DeleteCollider(&collider_);
	}
}
