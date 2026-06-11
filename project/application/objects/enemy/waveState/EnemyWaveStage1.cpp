#include "EnemyWaveStage1.h"

#include "../EnemyManager.h"
#include "EnemyWaveStage2.h"

EnemyWaveStage1::EnemyWaveStage1(EnemyManager* _pEnemyManager) : EnemyWaveState("Stage1", _pEnemyManager)
{
}

void EnemyWaveStage1::Initialize()
{

	// EnemyManagerから共有LevelDataを受け取る
	SetLevelData(pEnemyManager_->GetLevelData());

	currentWave_ = 1;

	showWaveStartSprite_ = true;
	
	// ウェーブ開始エフェクト
	EffectIntro();

}

void EnemyWaveStage1::Update()
{
	// テクスチャ更新
	UpdateTexture();

	// ウェーブ開始エフェクト更新
	EffectUpdate();

	// 敵の発生コマンドを更新
	UpdateEnemyPopCommands(pEnemyManager_);

	// ステート切り替え
	if (pEnemyManager_->IsWaveChange())
	{
		// ステート遷移
		pEnemyManager_->ChangeState(std::make_unique<EnemyWaveStage2>(pEnemyManager_));
	}
}

void EnemyWaveStage1::Draw2D()
{
	if (showWaveStartSprite_)
	{
		for (auto& sprite : waveStartSprites_)
		{
			sprite->Draw();
		}
	}
}
