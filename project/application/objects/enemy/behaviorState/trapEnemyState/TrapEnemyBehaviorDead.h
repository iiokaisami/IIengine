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

private:

	// 死亡モーションのフレーム数
	uint32_t deadFrames_ = 30;

	// 最初の膨らみの大きさ
	float startPopScale_ = 1.8f;
	// 最終的な縮みの大きさ
	float endScale_ = 0.0f;
	// 上昇量
	float floatUpAmount_ = 0.1f;
	// 回転速度
	float rotSpeedY_ = 0.1f;
	float rotSpeedX_ = 0.1f;

	// パーティクルの数
	int explosionParticleCount_ = 6;
	int ruptureParticleCount_ = 20;

};

