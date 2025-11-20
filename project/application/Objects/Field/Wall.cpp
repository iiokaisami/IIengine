#include "Wall.h"

void Wall::Initialize()
{
	// --- 3Dオブジェクト ---
	object_ = std::make_unique<Object3d>();
	object_->Initialize("wall.obj");
	position_ = { 0.0f,0.0f,-0.5f };
	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	scale_ = { 1.5f,3.0f,1.0f };
	object_->SetScale(scale_);
	
	// ライト設定
	object_->SetDirectionalLightEnable(true);

	// 当たり判定
	colliderManager_ = ColliderManager::GetInstance();

	objectName_ = "Wall";
	desc =
	{
		//ここに設定
		.owner = this,
		.colliderID = objectName_,
		.shape = Shape::AABB,
		.shapeData = &aabb_,
		.attribute = colliderManager_->GetNewAttribute(objectName_),
	};
	
	collider_.MakeAABBDesc(desc);
	colliderManager_->RegisterCollider(&collider_);
}

void Wall::Finalize()
{
	colliderManager_->DeleteCollider(&collider_);
}

void Wall::Update()
{
	// オブジェクトの更新
	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	object_->SetScale(scale_);
	object_->Update();

	// 当たり判定更新
	aabb_.min = position_ - object_->GetScale();
	aabb_.max = position_ + object_->GetScale();
	aabb_.max.y += 1.0f; // 見た目よりも縦に大きく
	collider_.SetPosition(position_);
}

void Wall::Draw()
{
	object_->Draw();
}

void Wall::ImGuiDraw()
{
#ifdef USE_IMGUI

	ImGui::Begin("Wall");

	ImGui::SliderFloat3("pos", &position_.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("rot", &rotation_.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("scale", &scale_.x, 0.0f, 10.0f);
	
	ImGui::End();

#endif // USE_IMGUI
}

void Wall::OnCollisionTrigger()
{
	// とりあえず何もしない
}
