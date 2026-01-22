#pragma once

#include "TrapEnemyBehaviorState.h"

/// <summary>
/// 罠設置型敵の移動状態
/// </summary>
class TrapEnemyBehaviorMove : public TrapEnemyBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pTrapEnemy">罠型敵のポインタ</param>
	TrapEnemyBehaviorMove(TrapEnemy* _pTrapEnemy);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;
	
	// モーションのリセット
	void ResetMotion() override;


private:

	// 罠設置のクールタイム	
	uint32_t trapCooldown_ = 0;
	// クールタイムの最大値
	const uint32_t kMaxTrapCooldown = 60 * 3;

};

