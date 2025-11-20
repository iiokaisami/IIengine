#include "Field.h"

void Field::Initialize()
{
	// --- 3Dオブジェクト ---
	object_ = std::make_unique<Object3d>();
	object_->Initialize("field.obj");
	
	position_ = { 0.0f,0.0f,0.0f };
	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	scale_ = { 100.0f,0.1f,100.0f };
	object_->SetScale(scale_);
	// ライト設定
	object_->SetDirectionalLightEnable(true);

	// 当たり判定
	colliderManager_ = ColliderManager::GetInstance();
	objectName_ = "Field";
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

void Field::Finalize()
{
	colliderManager_->DeleteCollider(&collider_);
}

void Field::Update()
{
	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	object_->SetScale(scale_);
	object_->Update();

	aabb_.min = position_ - object_->GetScale();
	aabb_.max = position_ + object_->GetScale();
	aabb_.max.y += 1.0f;
	collider_.SetPosition(position_);
}

void Field::Draw()
{
	object_->Draw();
}

void Field::ImGuiDraw()
{
#ifdef USE_IMGUI

	ImGui::Begin("Field");

	ImGui::SliderFloat3("pos", &position_.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("rot", &rotation_.x, -3.14f, 3.14f);
	ImGui::SliderFloat3("scale", &scale_.x, 0.0f, 10.0f);
	
	ImGui::End();

#endif // USE_IMGUI
}

void Field::OnCollisionTrigger()
{
	// 何もしない
}
