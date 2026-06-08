#include "EnemyWaveStage3.h"

#include "../EnemyManager.h"

EnemyWaveStage3::EnemyWaveStage3(EnemyManager* _pEnemyManager) : EnemyWaveState("Stage3", _pEnemyManager)
{
	LoadTextureWave3();
}

void EnemyWaveStage3::Initialize()
{
	// EnemyManagerから共有LevelDataを受け取る
	SetLevelData(pEnemyManager_->GetLevelData());

	currentWave_ = 3;
	
	showWaveStartSprite_ = true;
	waveStartSpriteTimer_ = kWaveStartSpriteDuration_;
	spritePos_ = { 1000,0 };
	
	// ウェーブ開始エフェクト
	EffectIntro();

	// ガーディアンを出現
	pEnemyManager_->GuardianInit({ 30.0f, 0.5f, 20.0f });

}

void EnemyWaveStage3::Update()
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
		// 今はこのウェーブをクリアしたらゲームクリア
		pEnemyManager_->SetAllEnemyDefeated(true);
	}

}

void EnemyWaveStage3::Draw2D()
{
	if (showWaveStartSprite_)
	{
		for (auto& sprite : waveStartSprites_)
		{
			sprite->Draw();
		}
	}
}
