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

};

