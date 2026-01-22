#pragma once

#include "TrapEnemyBehaviorState.h"

/// <summary>
/// 罠設置状態
/// </summary>
class TrapEnemyBehaviorSetTrap : public TrapEnemyBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pTrapEnemy">罠型敵のポインタ</param>
	TrapEnemyBehaviorSetTrap(TrapEnemy* _pTrapEnemy);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;
	
	// モーションのリセット
	void ResetMotion() override;


private:

	// 罠設置フラグ
	bool isSetTrap_ = false;

};

