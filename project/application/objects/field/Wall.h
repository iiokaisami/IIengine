#pragma once

#include "../../baseObject/StaticObject.h"
#include"../../../gameEngine/collider/ColliderManager.h"

#include <Object3d.h>
#include <memory>
#include <Framework.h>

/// <summary>
/// 壁クラス
/// </summary>
class Wall : public StaticObject
{
public:

	Wall() = default;
	~Wall() = default;

	// 初期化
	void Initialize() override;
	
	// ImGui
	void ImGuiDraw();

private: // 衝突判定

	// 衝突時コールバック
	void OnCollisionTrigger();

private:

	// デフォルト値
	Vector3 wallInitialPos_ = { 0.0f,0.0f,-0.5f };
	Vector3 wallInitialScale_ = { 1.5f,3.0f,1.0f };

};

