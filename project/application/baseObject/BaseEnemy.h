#pragma once

#include "Character.h"

#include <memory>
#include <queue>
#include <functional>
#include <Vector3.h>
#include <Framework.h>
#include <unordered_set>

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
	void SetObstaclePositions(const std::vector<Vector3>& _obstaclePositions) { obstaclePositions_ = _obstaclePositions; }

protected:

	// 進行方向の補間処理
	Vector3 InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate);

	// 障害物回避用パス計算
	std::vector<Vector3> CalculatePath(const Vector3& start, const Vector3& goal, const std::vector<Vector3>& obstacles);

	// 障害物の近傍ノードを取得
	std::vector<Vector3> GetNeighbors(const Vector3& node);

	// ヒューリスティック関数
	float Heuristic(const Vector3& a, const Vector3& b);

	struct GridPos
	{
		int x;
		int z;

		bool operator==(const GridPos& other) const
		{
			return x == other.x && z == other.z;
		}
	};

	struct GridPosHash
	{
		size_t operator()(const GridPos& p) const noexcept
		{
			return std::hash<int>()(p.x) ^ (std::hash<int>()(p.z) << 1);
		}
	};

	GridPos ToGrid(const Vector3& v) const;
	Vector3 ToWorld(const GridPos& g) const;

	std::vector<Vector3> GetNeighborsWorld(const GridPos& node);

protected:

	// 移動関連
	Vector3 moveVelocity_ = {};
	float moveSpeed_ = 0.0f;

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


};