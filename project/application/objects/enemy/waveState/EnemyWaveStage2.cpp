#include "EnemyWaveStage2.h"

#include "../EnemyManager.h"

EnemyWaveStage2::EnemyWaveStage2(EnemyManager* _pEnemyManager) : EnemyWaveState("Stage1", _pEnemyManager)
{
}

void EnemyWaveStage2::Initialize()
{

	// EnemyManagerから共有LevelDataを受け取る
	SetLevelData(pEnemyManager_->GetLevelData());

	currentWave_ = 2;
}

void EnemyWaveStage2::Update()
{
	// 敵の発生コマンドを更新
	UpdateEnemyPopCommands(pEnemyManager_);

	// ステート切り替え
	if (pEnemyManager_->IsWaveChange())
	{
		// ステート遷移3を作ったらここに追記

		// 今はこのウェーブをクリアしたらゲームクリア
		pEnemyManager_->SetAllEnemyDefeated(true);
	}
}
