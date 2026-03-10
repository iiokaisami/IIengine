#pragma once

#include "EnemyBehaviorState.h"

/// <summary>
/// 被弾リアクション
/// </summary>
class EnemyBehaviorHitReact : public EnemyBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pNormalEnemy">通常敵ポインタ</param>
	EnemyBehaviorHitReact(NormalEnemy* _pNormalEnemy);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのリセット
	void ResetMotion() override;

private:

	// 被弾リアクションのフレーム数
	uint32_t hitReactFrames_ = 30;

	// 振動の強さ
	float shakeAmp_ = 0.1f;
	// 振動の周期
	int shakePeriodX_ = 2;
	// 振動の周期
	int shakePeriodZ_ = 3;

};

