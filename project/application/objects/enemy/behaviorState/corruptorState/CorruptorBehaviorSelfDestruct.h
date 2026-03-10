#pragma once

#include "CorruptorBehaviorState.h"

class CorruptorBehaviorSelfDestruct : public CorruptorBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pCorruptor">コラプターのポインタ</param>
	CorruptorBehaviorSelfDestruct(Corruptor* _pCorruptor);

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// モーションのリセット
	void ResetMotion() override;

private:

	// 自爆モーション時間
	uint32_t selfDestructMotionFrames_ = 60;
	
	// はじけ開始スケール
	float selfDestructScaleFrom_ = 1.0f;
	// はじけ最大スケール
	float selfDestructScaleTo_ = 2.0f;

	// 揺れ
	float selfDestructShakeAmp_ = 0.1f;
	int selfDestructShakePeriodX_ = 2;
	int selfDestructShakePeriodZ_ = 3;

	// 自爆時パーティクル量
	int particleExplosionGroupCount_ = 15;
	int particleRuptureCount_ = 20;
	int particleBulletReactionCount_ = 1;

};

