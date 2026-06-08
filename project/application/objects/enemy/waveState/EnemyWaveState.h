#pragma once

#include <sstream>

#include "../../../../gameEngine/level_editor/LevelDataLoader.h"
#include "postEffect/PostEffectManager.h"

#include <Sprite.h>

class EnemyManager;

/// <summary>
/// 通常敵のウェーブステート
/// 通常敵の発生を管理
/// </summary>
class EnemyWaveState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_wave">ウェーブ名</param>
	/// <param name="_pEnemyManager">エネミーマネージャーのポインタ</param>
	EnemyWaveState(const std::string& _wave, EnemyManager* _pEnemyManager) : wave_(_wave), pEnemyManager_(_pEnemyManager) {};
	
	virtual~EnemyWaveState();

	// 初期化
	virtual void Initialize() = 0;
	
	// 更新
	virtual void Update() = 0;

	// テクスチャの読み込み
	virtual void LoadTextureWave1();
	virtual void LoadTextureWave2();
	virtual void LoadTextureWave3();

	// テクスチャ更新
	virtual void UpdateTexture();

	// 2D描画
	virtual void Draw2D() = 0;

	// ウェーブ開始エフェクト
	void EffectIntro();

	// ウェーブ開始エフェクト更新
	void EffectUpdate();

public: // セッター

	/// <summary>
	/// エディタセット
	/// </summary>
	/// <param name="_levelData">レベルデータローダー</param>
	void SetLevelData(std::shared_ptr<IIEngine::LevelData> _levelData) { levelData_ = _levelData; }

protected:

	/// <summary>
	/// CSVファイルの読み込み
	/// </summary>
	/// <param name="csvPath">CSVファイルのパス</param>
	void LoadCSV(const std::string& csvPath);

	/// <summary>
	/// 敵の発生コマンドを更新
	/// </summary>
	/// <param name="_pEnemyManager">エネミーマネージャーのポインタ</param>
	void UpdateEnemyPopCommands(EnemyManager* _pEnemyManager);

	/// <summary>
	/// csvの更新
	/// </summary>
	/// <param name="_pEnemyManager">エネミーマネージャーのポインタ</param>
	void UpdateCSV(EnemyManager* _pEnemyManager);

	std::string wave_;
	EnemyManager* pEnemyManager_ = nullptr;

	
	// 敵待機タイマー
	uint32_t enemyWaitingTimer_ = 9;

	// レベルデータローダー
	std::shared_ptr<IIEngine::LevelData> levelData_ = nullptr;

	// 現在のウェーブ
	uint32_t currentWave_ = 1;

	// ウェーブ開始スプライト
	std::vector<std::unique_ptr<IIEngine::Sprite>> waveStartSprites_ = {};
	uint32_t spriteIndex_ = 2;
	Vector2 spritePos_ = { 1000,0 };
	bool showWaveStartSprite_ = true;
	uint32_t waveStartSpriteTimer_ = 180;
	const uint32_t kWaveStartSpriteDuration_ = 120;

	// 表示開始時の初期値保持
	float waveStartSpriteInitialX_ = 0.0f;
	uint32_t waveStartSpriteInitialTimer_ = 0;
	bool waveStartSpriteInit_ = false;

	// ウェーブ開始エフェクト
	float waveIntroTimer_ = 0.0f;
	bool isWaveIntro_ = false;
	bool isPulseTriggered_ = false;

	// 敵の発生コマンド
	std::stringstream enemyPopCommands_;
	// 敵機中フラグ
	bool isEnemyWaiting_ = true;
	// 確認用 csv更新用カウント
	uint32_t debugCount_ = 9;
};

