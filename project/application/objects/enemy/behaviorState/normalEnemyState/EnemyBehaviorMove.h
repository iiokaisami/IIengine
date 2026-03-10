#pragma once

#include "EnemyBehaviorState.h"

/// <summary>
/// 敵キャラクターの移動状態を管理するクラス
/// </summary>
class EnemyBehaviorMove : public EnemyBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pNormalEnemy">通常敵ポインタ</param>
	EnemyBehaviorMove(NormalEnemy* _pNormalEnemy);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのリセット
	void ResetMotion() override;

private:

	// 移動モーション
	uint32_t moveMotionFrames_ = 30;

	// 移動モーションの振幅
	float moveScaleWaveAmp_ = 0.2f;
	
	// 移動モーションの振幅の倍率
	float moveScaleZ_ = 1.0f;

};

