#include "EnemyBullet.h"

void EnemyBullet::Initialize()
{
	// --- 3Dオブジェクト ---
	object_ = std::make_unique<Object3d>();
	object_->Initialize("enemyBullet.obj");

	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	scale_ = { 0.7f,0.7f,0.7f };
	object_->SetScale(scale_);

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
		.onCollisionTrigger = std::bind(&EnemyBullet::OnCollisionTrigger, this, std::placeholders::_1),
	};
	collider_.MakeAABBDesc(desc);
	colliderManager_->RegisterCollider(&collider_);

}

void EnemyBullet::Finalize()
{
	colliderManager_->DeleteCollider(&collider_);
}

void EnemyBullet::Update()
{
	UpdateModel();

	rotation_ += {0.1f, 0.1f, 0.0f};
	position_ += velocity_;

	aabb_.min = position_ - object_->GetScale();
	aabb_.max = position_ + object_->GetScale();
	collider_.SetPosition(position_);

	// 残り寿命に応じてスケールを小さくする
	float t = static_cast<float>(deathTimer_) / static_cast<float>(kLifeTime);
	t = std::clamp(t, 0.0f, 1.0f);
	scale_ = { 0.7f * t, 0.7f * t, 0.7f * t };


	//時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw()
{
	object_->Draw();
}

void EnemyBullet::Draw2D()
{
}

void EnemyBullet::ImGuiDraw()
{
#ifdef USE_IMGUI

	ImGui::Begin("EnemyBullet");

	ImGui::SliderFloat3("position", &position_.x, -30.0f, 30.0f);
	ImGui::SliderFloat3("rotation", &rotation_.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("scale", &scale_.x, 0.0f, 10.0f);

	ImGui::End();

#endif // USE_IMGUI
}

void EnemyBullet::UpdateModel()
{
	object_->Update();

	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	object_->SetScale(scale_);
}

void EnemyBullet::OnCollisionTrigger(const Collider* _other)
{
	if (!_other->GetOwner()->IsActive() && _other->GetColliderID() == "Player" or
		_other->GetColliderID() == "PlayerBullet" or
		_other->GetColliderID() == "Wall" or
		_other->GetColliderID() == "TrapEnemy" or
		_other->GetColliderID() == "Barrie")
	{
		isDead_ = true;
	} 
}
