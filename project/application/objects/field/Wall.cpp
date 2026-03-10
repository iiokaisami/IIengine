#include "Wall.h"

void Wall::Initialize()
{
	// 基底クラス初期化
	StaticObject::Initialize("wall.obj", "Wall");

	position_ = wallInitialPos_;
	scale_ = wallInitialScale_;
	SyncObjectTransform();
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
