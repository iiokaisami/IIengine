#pragma once

#include "EnemyWaveState.h"

/// <summary>
///	敵のウェーブステート3
/// </summary>
class EnemyWaveStage3 : public EnemyWaveState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pEnemyManager">エネミーマネージャーのポインタ</param>
	EnemyWaveStage3(EnemyManager* _pEnemyManager);
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;
	
	// 2D描画
	void Draw2D() override;

};

