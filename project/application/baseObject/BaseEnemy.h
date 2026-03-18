#pragma once

#include "Character.h"

#include <memory>
#include <queue>
#include <functional>
#include <Vector3.h>
#include <Framework.h>
#include <unordered_set>
#include <string>

/// <summary>
/// 敵の基底クラス
/// </summary>
class BaseEnemy : public Character
{
public:

	BaseEnemy() = default;
	virtual ~BaseEnemy() = default;

	// 初期化
	virtual void Initialize(const std::string& _modelFileName, const std::string& _objectName, float _initialHP);

	// 更新
	virtual void Update() override;

	// 移動
	virtual void Move() override;

	// 障害物の位置リストをセット
	void SetObstaclePositions(std::vector<Vector3> _obstaclePositions) { obstaclePositions_ = std::move(_obstaclePositions); }

protected:

	// 進行方向の補間処理
	Vector3 InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate);

	// 障害物回避用パス計算
	std::vector<Vector3> CalculatePath(const Vector3& start, const Vector3& goal, const std::vector<Vector3>& obstacles);

	// 障害物の近傍ノードを取得
	std::vector<Vector3> GetNeighbors(const Vector3& node);

	// ヒューリスティック関数
	float Heuristic(const Vector3& a, const Vector3& b);

	// グリッド座標
	struct GridPos
	{
		int x;
		int z;

		bool operator==(const GridPos& other) const
		{
			return x == other.x && z == other.z;
		}
	};

	// グリッド座標のハッシュ関数
	struct GridPosHash
	{
		size_t operator()(const GridPos& p) const noexcept
		{
			return std::hash<int>()(p.x) ^ (std::hash<int>()(p.z) << 1);
		}
	};

	/// <summary>
	/// ワールド座標をグリッド座標に変換
	/// </summary>
	/// <param name="v">ワールド座標</param>
	/// <returns>グリッド座標</returns>
	GridPos ToGrid(const Vector3& v) const;

	/// <summary>
	/// グリッド座標をワールド座標に変換
	/// </summary>
	/// <param name="g">グリッド座標</param>
	/// <returns>ワールド座標</returns>
	Vector3 ToWorld(const GridPos& g) const;

	/// <summary>
	/// グリッド座標の近傍ノードをワールド座標で取得
	/// </summary>
	/// <param name="node">グリッド座標のノード</param>
	/// <returns>近傍ノードのワールド座標リスト</returns>
	std::vector<Vector3> GetNeighborsWorld(const GridPos& node);

	/// <summary>
	/// スタートからエンドまでの直線上に障害物がないかどうかを判定
	/// </summary>
	/// <param name="start">スタートのグリッド座標</param>
	/// <param name="end">エンドのグリッド座標</param>
	/// <returns>障害物がないかどうか</returns>
	bool HasLineOfSight(const GridPos& start, const GridPos& end);

	/// <summary>
   /// パスをスムージングして余計なウェイポイントを削減
   /// </summary>
   /// <param name="path">元のパス</param>
   /// <returns>スムージングされたパス</returns>
	std::vector<Vector3> SmoothPath(const std::vector<Vector3>& path);

protected:

	// 移動関連
	Vector3 moveVelocity_ = {};
	float moveSpeed_ = 0.0f;
	// パラメータ
	float moveFacingMinVelocity_ = 0.05f;
	float directionEpsilon_ = 0.001f;
	// デフォルト値
	Vector3 moveVelocityDefault_ = { 0.1f, 0.0f, 0.1f };
	float moveSpeedDefault_ = 0.05f;


	// プレイヤー関連
	Vector3 playerPosition_ = {};
	Vector3 toPlayer_ = {};
	Vector3 lastPlayerPos_ = {};

	// フレームレート補助
	static constexpr float kDefaultFrameRate = 60.0f;

	// 障害物の位置リスト
	std::vector<Vector3> obstaclePositions_ = {};

	// 探索済みノードセット
	std::unordered_set<GridPos, GridPosHash> closedSet_ = {};

	// 現在のパス
	std::vector<Vector3> path_;
	// 現在のウェイポイントインデックス
	size_t currentWaypointIndex_ = 0;
	// パスが汚れているかどうか
	bool pathDirty_ = true;
	// パスに沿って移動中かどうか
	bool isFollowingPath_ = false;

	enum class MoveMode
	{
		Direct,   // 障害物なし
		FollowPath // A*のパスを追従中
	};
	MoveMode moveMode_ = MoveMode::Direct;

	// ループ回数の上限
	std::unordered_set<GridPos, GridPosHash> obstacleGrid_;

	// 回転補間のレート
	float yawLerpRate_ = 0.2f; 

	// A*のループ回数上限
	int maxPathSearchLoop_ = 1000;

	// グリッドのサイズとオフセット
	float gridCellSize_ = 1.0f;    
	float gridCellCenterOffset_ = 0.5f;
	float gridWorldY_ = 0.0f;

};