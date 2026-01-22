#pragma once

#include "TrapEnemyBehaviorState.h"

/// <summary>
/// 罠型敵の被弾状態
/// </summary>
class TrapEnemyBehaviorHitReact : public TrapEnemyBehaviorState
{
public:
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pTrapEnemy">罠型敵のポインタ</param>
	TrapEnemyBehaviorHitReact(TrapEnemy* _pTrapEnemy);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのリセット
	void ResetMotion() override;

};

