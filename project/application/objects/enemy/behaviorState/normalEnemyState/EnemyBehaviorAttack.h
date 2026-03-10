#pragma once

#include "EnemyBehaviorState.h"

/// <summary>
/// 通常敵の攻撃状態
/// </summary>
class EnemyBehaviorAttack : public EnemyBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pNormalEnemy">通常敵ポインタ</param>
	EnemyBehaviorAttack(NormalEnemy* _pNormalEnemy);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのリセット
	void ResetMotion() override;


private:

	// 攻撃フラグ
	bool isAttack_ = false;
	// 攻撃クールタイム
	int attackCooldown_ = 0;

	// 攻撃モーション
	uint32_t attackMotionFrames_ = 30;

	// 溜めモーション
	uint32_t chargeFrames_ = 20;
	float chargeScaleShrink_ = 0.3f;

	// 溜めのときの振動
	float chargeShakeAmp_ = 0.05f;
	int chargeShakePeriod_ = 2;

	// 攻撃のときの回転
	float attackYawStep_ = 0.3f;
	uint32_t attackTriggerFrame_ = 21;

	// 攻撃後のクールダウン
	int initialCooldownFrames_ = 10;
	int cooldownFrames_ = 60 * 3;

};

