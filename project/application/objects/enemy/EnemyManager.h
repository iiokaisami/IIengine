#pragma once

#include <vector>
#include <Framework.h>

#include "NormalEnemy.h"
#include "TrapEnemy.h"
#include "Corruptor.h"
#include "Guardian.h"
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

	// スプライト描画
	void Draw2D();

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
	/// ガーディアンの初期化
	/// </summary>
	/// <param name="pos">出現位置</param>
	void GuardianInit(const Vector3& pos);

	/// <summary>
	/// 敵のウェーブステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステート</param>
	void ChangeState(std::unique_ptr<EnemyWaveState>_pState);

	/// <summary>
	/// 最も近い敵の位置を取得
	/// </summary>
	/// <param name="from">基準位置</param>
	/// <param name="outPos">最も近い敵の位置</param>
	/// <returns>最も近い敵が存在するかどうか</returns>
	bool GetNearestEnemyPosition(const Vector3& from, Vector3& outPos) const;


public: // ゲッター

	// プレイヤーとの距離のゲッター
	const std::vector<Vector3>& GetToPlayerDistance() const { return toPlayerDistance_; }

	// 敵のウェーブ切り替えフラグ
	bool IsWaveChange() const { return isWaveChange_; }

	// 全てのウェーブの敵を倒したかどうかのフラグ
	bool IsAllEnemyDefeated() const { return isAllEnemyDefeated_; }

	// レベルエディタ
	std::shared_ptr<IIEngine::LevelData> GetLevelData() const { return levelData_; }

	// 障害物の位置リスト取得
	const std::vector<Vector3>& GetObstacles() const { return obstacles_; }

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
	void SetLevelData(std::shared_ptr<IIEngine::LevelData> _levelData) { levelData_ = _levelData; }

	/// <summary>
	/// 障害物の位置リストセット
	/// </summary>
	/// <param name="obstacles">障害物の位置ベクトル配列</param>
	void SetObstacles(const std::vector<Vector3>& _obstacles) { obstacles_ = _obstacles; }

    /// <summary>
    /// ウェーブチェンジフラグセット
    /// </summary>
	void SetIsWaveChange(bool _isWaveChange) { isWaveChange_ = _isWaveChange; }

private:

	// エネミー
	std::vector<std::unique_ptr<NormalEnemy>> pNormalEnemies_;
	// トラップエネミー
	std::vector<std::unique_ptr<TrapEnemy>> pTrapEnemies_;
	// コラプター
	std::vector<std::unique_ptr<Corruptor>> pCorruptors_;
	// ガーディアン
	std::vector<std::unique_ptr<Guardian>> pGuardians_;

	// 敵の数
	uint32_t enemyCount_ = 0;

	// 敵のウェーブステート
	std::unique_ptr<EnemyWaveState> pState_ = nullptr;
	// 敵のウェーブ切り替え
	bool isWaveChange_ = false;
	// 敵のウェーブ切り替えインターバル
	int32_t waveChangeInterval_ = 250;

	// プレイヤーの位置
	Vector3 playerPosition_{};
	// プレイヤーとの距離
	std::vector<Vector3> toPlayerDistance_;

	// 全てのウェーブの敵を倒したら立てるフラグ
	bool isAllEnemyDefeated_ = false;

	// レベルデータローダー
	std::shared_ptr<IIEngine::LevelData> levelData_ = nullptr;

	// 障害物の位置リスト
	std::vector<Vector3> obstacles_ = {};

	// フレームレートの基準値
	float updateFrameRateBase_ = 60.0f;

	// ウェーブ切り替えインターバルの初期値とリセット値
	int32_t waveChangeIntervalInitFrames_ = 250;
	int32_t waveChangeIntervalResetFrames_ = 120;

	// タイトルシーン用の敵のスポーン位置
	Vector3 gameOverEnemySpawnPos_ = { -20.0f, 0.5f, 0.0f };
	std::vector<Vector3> titleEnemySpawnPos_ =
	{
		{ 2.0f, 0.5f, -5.0f },
		{ -2.0f, 0.5f, 2.0f },
		{ -5.0f, 0.5f, -2.0f },
		{ 5.0f, 0.5f, 5.0f },
	};

};

