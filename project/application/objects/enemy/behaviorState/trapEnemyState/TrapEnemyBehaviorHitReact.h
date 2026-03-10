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

private:
	
	// ヒットリアクションの持続フレーム数
	uint32_t hitReactFrames_ = 30;

	// 震えの大きさ
	float shakeAmp_ = 0.1f;
	int shakePeriodX_ = 2;
	int shakePeriodZ_ = 3;

};

