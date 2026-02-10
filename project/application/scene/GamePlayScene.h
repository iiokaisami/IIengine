#pragma once

#include <Framework.h>

#include "../../gameEngine/baseScene/BaseScene.h"

/// <summary>
///	ゲームプレイシーン
/// プレイヤー操作、敵出現、フィールド、ゴール、衝突判定、シーン遷移
/// </summary>
class GamePlayScene : public IIEngine::BaseScene
{
public:

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	
};