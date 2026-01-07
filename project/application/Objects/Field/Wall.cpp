#include "Wall.h"

void Wall::Initialize()
{
	// 基底クラス初期化
	StaticObject::Initialize("wall.obj", "Wall");

	position_ = { 0.0f,0.0f,-0.5f };
	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	scale_ = { 1.5f,3.0f,1.0f };
	object_->SetScale(scale_);
	
	// ライト設定
	object_->SetLighting(true);

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
