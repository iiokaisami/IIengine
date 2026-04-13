#include "Field.h"

void Field::Initialize()
{
	
	// 基底クラス初期化
	StaticObject::Initialize("field.obj", "Field");

	position_ = { 0.0f,0.0f,0.0f };
	scale_ = fieldScale_;
	SyncObjectTransform();
}

void Field::Update()
{
	// パーティクル
	IIEngine::ParticleEmitter::Emit("quadRise", position_, particleCount_);
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
