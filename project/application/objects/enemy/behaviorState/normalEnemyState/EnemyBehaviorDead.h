#pragma once

#include "EnemyBehaviorState.h"

/// <summary>
/// 敵の行動ステート：死亡
/// </summary>
class EnemyBehaviorDead : public EnemyBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pNormalEnemy">通常敵ポインタ</param>
	EnemyBehaviorDead(NormalEnemy* _pNormalEnemy);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのリセット
	void ResetMotion() override;

private:

	// 死亡モーションのフレーム数
	uint32_t deadFrames_ = 30;

	// 初回膨張
	float startPopScale_ = 1.8f;     
	// 最終縮小
	float endScale_ = 0.0f;          
	// 上昇量
	float floatUpAmount_ = 0.1f;
	// 回転速度
	float rotSpeedY_ = 0.1f;
	float rotSpeedX_ = 0.1f;

	// パーティクルの数
	int petalParticleCount_ = 6;
	int ruptureParticleCount_ = 20;

};

