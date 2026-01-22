#include "PlayerBullet.h"
#include "TimeManager.h"

void PlayerBullet::Initialize()
{
	// --- 3Dオブジェクト ---
  	object_ = std::make_unique<Object3d>();
	object_->Initialize("playerBullet.obj");

	scale_ = { 0.5f,0.5f,0.5f };
	SyncObjectTransform();
	// ライト設定
	object_->SetLighting(true);

	// 当たり判定
	colliderManager_ = ColliderManager::GetInstance();

	objectName_ = "PlayerBullet";

	desc =
	{
		//ここに設定
		.owner = this,
		.colliderID = objectName_,
		.shape = Shape::AABB,
		.shapeData = &aabb_,
		.attribute = colliderManager_->GetNewAttribute(objectName_),
		.onCollisionTrigger = std::bind(&PlayerBullet::OnCollisionTrigger, this, std::placeholders::_1),
	};
	collider_.MakeAABBDesc(desc);
	colliderManager_->RegisterCollider(&collider_);



}

void PlayerBullet::Finalize()
{
	colliderManager_->DeleteCollider(&collider_);
}

void PlayerBullet::Update()
{
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	BaseBullet::Update();

	SyncObjectTransform();

	rotation_.y += 1.0f * dt * PlayerBullet::kDefaultFrameRate;
	position_ += velocity_ * dt;

	aabb_.min = position_ - object_->GetScale();
	aabb_.max = position_ + object_->GetScale();
	collider_.SetPosition(position_);

	// パーティクル
	IIEngine::ParticleEmitter::Emit("slash", position_, 1);
}

void PlayerBullet::Draw()
{
	object_->Draw();
}

void PlayerBullet::Draw2D()
{
}

void PlayerBullet::ImGuiDraw()
{
#ifdef USE_IMGUI

	ImGui::Begin("PlayerBullet");
	
	ImGui::SliderFloat3("position", &position_.x, -30.0f, 30.0f);
	ImGui::SliderFloat3("rotation", &rotation_.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("scale", &scale_.x, 0.0f, 10.0f);
	
	ImGui::End();

#endif // USE_IMGUI
}

void PlayerBullet::OnCollisionTrigger(const Collider* _other)
{
	if (_other->GetColliderID() != "Player" && _other->GetColliderID() != "Field")
	{
		isDead_ = true;
	}  
}