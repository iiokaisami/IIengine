#pragma once

#include "TrapEnemyBehaviorState.h"

/// <summary>
/// 罠設置型敵の死亡状態
/// </summary>
class TrapEnemyBehaviorDead : public TrapEnemyBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pTrapEnemy">罠型敵のポインタ</param>
	TrapEnemyBehaviorDead(TrapEnemy* _pTrapEnemy);

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// モーションのリセット
	void ResetMotion() override;


};

