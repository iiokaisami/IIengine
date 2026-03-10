#pragma once

#include "CorruptorBehaviorState.h"

class CorruptorBehaviorMove : public CorruptorBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pCorruptor">コラプターのポインタ</param>
	CorruptorBehaviorMove(Corruptor* _pCorruptor);

	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;
	
	// モーションのリセット
	void ResetMotion() override;

private:

	// 移動モーション時間
	uint32_t moveMotionFrames_ = 30;
	// ポヨポヨ振幅
	float moveScaleWaveAmp_ = 0.2f;
	// Zスケール
	float moveScaleZ_ = 1.0f;

};

