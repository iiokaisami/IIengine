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

	// 攻撃モーション
	uint32_t setTrapMotionFrames_ = 30;

	// 膨らむモーション
	uint32_t expandFrames_ = 15;
	float expandScaleAdd_ = 0.5f;

	// 震えモーション
	float shakeMag_ = 0.05f;
	float shakeFreqX_ = 0.5f;
	float shakeFreqY_ = 0.7f;

	// 罠設置フラグ
	bool isSetTrap_ = false;

};

