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
		object_->SetPosition(position_);
		object_->SetRotate(rotation_);
		object_->SetScale(scale_);
		object_->Update();
	}
}

void BaseBullet::Draw()
{
	if (object_)
	{
		object_->Draw();
	}
}

void BaseBullet::UpdateModel()
{
	if (object_)
	{
		object_->SetPosition(position_);
		object_->SetRotate(rotation_);
		object_->SetScale(scale_);
		object_->Update();
	}
}

Vector3 BaseBullet::SetVelocity(const Vector3 _velocity)
{
	// フレームレート補正
	velocity_ = _velocity * kDefaultFrameRate;
	return velocity_;
}

void BaseBullet::UpdateLifetime()
{
	const float dt = TimeManager::Instance().GetDeltaTime();
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
