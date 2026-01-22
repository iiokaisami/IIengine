#pragma once

#include "EnemyWaveState.h"

/// <summary>
/// 敵のウェーブステート
/// ウェーブ2
/// </summary>
class EnemyWaveStage2 : public EnemyWaveState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pEnemyManager">エネミーマネージャーのポインタ</param>
	EnemyWaveStage2(EnemyManager* _pEnemyManager);

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;
};

