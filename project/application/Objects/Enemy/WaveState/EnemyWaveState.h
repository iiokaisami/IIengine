#pragma once

#include <sstream>

#include "../../../../gameEngine/level_editor/LevelDataLoader.h"

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



	// 敵の発生コマンド
	std::stringstream enemyPopCommands_;
	// 敵機中フラグ
	bool isEnemyWaiting_ = true;
	// 確認用csv更新用カウント
	uint32_t debugCount_ = 9;
};

