#pragma once

#include "../../baseObject/StaticObject.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <memory>
#include <Framework.h>

/// <summary>
/// フィールド
/// </summary>
class Field : public StaticObject
{
public:
	
	Field() = default;
	~Field() = default;
	
	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// ImGui
	void ImGuiDraw();

private: // 衝突判定

	/// <summary>
	/// 衝突時処理
	/// </summary>
	void OnCollisionTrigger();

private:

	// デフォルト値
	Vector3 fieldScale_ = { 100.0f, 0.1f, 100.0f };

	// パーティクル数
	uint32_t particleCount_ = 1;

};