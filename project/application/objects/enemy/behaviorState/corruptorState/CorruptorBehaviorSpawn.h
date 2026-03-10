#pragma once

#include "CorruptorBehaviorState.h"

/// <summary>
/// コラプターの出現状態
/// </summary>
class CorruptorBehaviorSpawn : public CorruptorBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pCorruptor">コラプターのポインタ</param>
	CorruptorBehaviorSpawn(Corruptor* _pCorruptor);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;
	
	// モーションのリセット
	void ResetMotion() override;

private:

	// スポーンモーションのフレーム数
	uint32_t spawnMotionFrames_ = 30;
	// スポーンモーションの持続時間
	float spawnTurns_ = 2.0f;

};

