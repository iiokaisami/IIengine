#include "ShockWave.h"
#include "TimeManager.h"

using namespace IIEngine;

void ShockWave::Initialize()
{
	// --- 3Dオブジェクト ---
	object_ = std::make_unique<Object3d>();
	object_->Initialize("shockWave.obj");

	scale_ = { baseScale_,1.0f,baseScale_ };
	SyncObjectTransform();

	object_->SetLighting(true);

	// 当たり判定
	colliderManager_ = ColliderManager::GetInstance();
	objectName_ = "EnemyBullet";

	desc =
	{
		//ここに設定
		.owner = this,
		.colliderID = objectName_,
		.shape = Shape::AABB,
		.shapeData = &aabb_,
		.attribute = colliderManager_->GetNewAttribute(objectName_),
		.onCollisionTrigger = std::bind(&ShockWave::OnCollisionTrigger, this, std::placeholders::_1),
	};
	collider_.MakeAABBDesc(desc);
	colliderManager_->RegisterCollider(&collider_);

	lifeTimeFrames_ = kLifeTime;

}

void ShockWave::Finalize()
{
	colliderManager_->DeleteCollider(&collider_);
}

void ShockWave::Update()
{
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	SyncObjectTransform();

	BaseBullet::Update();

	// 徐々にx,zのスケールを大きくしていく
	scale_.x += expansionSpeed_ * dt * kDefaultFrameRate;
	scale_.z += expansionSpeed_ * dt * kDefaultFrameRate;

	// コライダー更新
	aabb_.min = position_ - object_->GetScale();
	aabb_.max = position_ + object_->GetScale();
	collider_.SetPosition(position_);

	// 消滅フラグがたったら徐々に小さくしていく
	if (isFading_)
	{
		scale_.y -= 0.01f; // 徐々に小さくする
		if (scale_.y < 0.05f) // 十分小さくなったら消滅
		{
			isDead_ = true;
		}
	}
	else
	{
		// フラグが立っていない場合は寿命更新
		lifeTimeFrames_--;
		// 寿命が尽きたら消滅フラグを立てる
		if (lifeTimeFrames_ <= 0)
		{
			isFading_ = true;
		}
	}


}

void ShockWave::Draw()
{
	object_->Draw();
}

void ShockWave::ImGuiDraw()
{
#ifdef USE_IMGUI

	ImGui::Begin("EnemyBullet");

	ImGui::SliderFloat3("position", &position_.x, -30.0f, 30.0f);
	ImGui::SliderFloat3("rotation", &rotation_.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("scale", &scale_.x, 0.0f, 70.0f);

	ImGui::End();

#endif // USE_IMGUI
}

void ShockWave::OnCollisionTrigger(const IIEngine::Collider* _other)
{
	if (!_other->GetOwner()->IsActive() &&
		(_other->GetColliderID() == "Player") &&
		!hasCollided_)
	{
		isFading_ = true;
		hasCollided_ = true;
	}
}
