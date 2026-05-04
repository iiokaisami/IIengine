#include "EnemyWaveStage2.h"

#include "../EnemyManager.h"

EnemyWaveStage2::EnemyWaveStage2(EnemyManager* _pEnemyManager) : EnemyWaveState("Stage1", _pEnemyManager)
{
	LoadTextureWave2();
}

void EnemyWaveStage2::Initialize()
{

	// EnemyManagerから共有LevelDataを受け取る
	SetLevelData(pEnemyManager_->GetLevelData());

	currentWave_ = 2;

	showWaveStartSprite_ = true;
	waveStartSpriteTimer_ = kWaveStartSpriteDuration_;
	spritePos_ = { 1000,0 };

	// ウェーブ開始エフェクト
	EffectIntro();

	// ガーディアンを出現(デバッグ用処理)
	pEnemyManager_->GuardianInit({30.0f, 0.5f, 20.0f});

}

void EnemyWaveStage2::Update()
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
		// ステート遷移3を作ったらここに追記

		// 今はこのウェーブをクリアしたらゲームクリア
		pEnemyManager_->SetAllEnemyDefeated(true);
	}
}

void EnemyWaveStage2::Draw2D()
{
	if (showWaveStartSprite_)
	{
		for (auto& sprite : waveStartSprites_)
		{
			sprite->Draw();
		}
	}
}
