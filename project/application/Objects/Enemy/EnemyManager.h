#pragma once

#include <vector>
#include <Framework.h>

#include "NormalEnemy.h"
#include "TrapEnemy.h"
#include "Corruptor.h"
#include "waveState/EnemyWaveState.h"
#include "../../../gameEngine/level_editor/LevelDataLoader.h"

/// <summary>
/// 敵マネージャー
/// </summary>
class EnemyManager
{
public:

	EnemyManager() = default;
	~EnemyManager() = default;

	// 初期化
	void Initialize();

	// 終了
	void Finalize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// ImGui
	void ImGuiDraw();

	// タイトルシーン用の敵初期化
	void TitleEnemyInit();

	// タイトルシーン用の敵更新
	void TitleEnemyUpdate();

	// ゲームオーバーシーン用の敵初期化
	void GameOverEnemyInit();

	// ゲームオーバーシーン用の敵更新
	void GameOverEnemyUpdate();

	// 敵の初期化
	/// <summary>
	/// 通常敵の初期化
	/// </summary>
	/// <param name="pos">出現位置</param>
	void NormalEnemyInit(const Vector3& pos);
	/// <summary>
	/// トラップ敵の初期化
	/// </summary>
	/// <param name="pos">出現位置</param>
	void TrapEnemyInit(const Vector3& pos);
	/// <summary>
	/// コラプターの初期化
	/// </summary>
	/// <param name="pos">出現位置</param>
	void CorruptorInit(const Vector3& pos);

	/// <summary>
	/// 敵のウェーブステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステート</param>
	void ChangeState(std::unique_ptr<EnemyWaveState>_pState);

public: // ゲッター

	// プレイヤーとの距離のゲッター
	const std::vector<Vector3>& GetToPlayerDistance() const { return toPlayerDistance_; }

	// 敵のウェーブ切り替えフラグ
	bool IsWaveChange() const { return isWaveChange_; }

	// 全てのウェーブの敵を倒したかどうかのフラグ
	bool IsAllEnemyDefeated() const { return isAllEnemyDefeated_; }

	// レベルエディタ
	std::shared_ptr<LevelData> GetLevelData() const { return levelData_; }

public: // セッター

	/// <summary>
	/// プレイヤーの位置をセット
	/// </summary>
	/// <param name="playerPosition">プレイヤーの位置ベクトル</param>
	void SetPlayerPosition(const Vector3& playerPosition) { playerPosition_ = playerPosition; }

	/// <summary>
	/// 全てのウェーブの敵を倒したら立てるフラグセット
	/// </summary>
	/// <param name="isDefeated">フラグ</param>
	void SetAllEnemyDefeated(bool isDefeated) { isAllEnemyDefeated_ = isDefeated; }

	/// <summary>
	/// エディタセット
	/// </summary>
	/// <param name="_levelData">レベルデータローダー</param>
	void SetLevelData(std::shared_ptr<LevelData> _levelData) { levelData_ = _levelData; }

private:

	// エネミー
	std::vector<std::unique_ptr<NormalEnemy>> pNormalEnemies_;
	// トラップエネミー
	std::vector<std::unique_ptr<TrapEnemy>> pTrapEnemies_;
	// コラプター
	std::vector<std::unique_ptr<Corruptor>> pCorruptors_;

	// 敵の数
	uint32_t enemyCount_ = 0;

	// 敵のウェーブステート
	std::unique_ptr<EnemyWaveState> pState_ = nullptr;
	// 敵のウェーブ切り替え
	bool isWaveChange_ = false;
	// 敵のウェーブ切り替えインターバル
	int32_t waveChangeInterval_ = 90;//= 250;

	// プレイヤーの位置
	Vector3 playerPosition_{};
	// プレイヤーとの距離
	std::vector<Vector3> toPlayerDistance_;

	// 全てのウェーブの敵を倒したら立てるフラグ
	bool isAllEnemyDefeated_ = false;

	// レベルデータローダー
	std::shared_ptr<LevelData> levelData_ = nullptr;

};

