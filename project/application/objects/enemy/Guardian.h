#pragma once

#include "../../baseObject/BaseEnemy.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Framework.h>

/// <summary>
/// ガーディアン
/// ボス的な存在
/// </summary>
class Guardian : public BaseEnemy
{
public:

	Guardian() = default;
	~Guardian() = default;

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	// ImGui
	void ImGuiDraw();

	// 移動
	void Move() override;



};

