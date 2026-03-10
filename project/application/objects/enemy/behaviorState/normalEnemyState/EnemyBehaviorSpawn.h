#pragma once

#include "EnemyBehaviorState.h"

/// <summary>
/// 敵の出現状態
/// </summary>
class EnemyBehaviorSpawn : public EnemyBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pNormalEnemy">通常敵ポインタ</param>
	EnemyBehaviorSpawn(NormalEnemy* _pNormalEnemy);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのリセット
	void ResetMotion() override;

private:

	// スポーンモーションのフレーム数
	uint32_t spawnMotionFrames_ = 30;

};

