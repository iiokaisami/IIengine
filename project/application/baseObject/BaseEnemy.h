#pragma once

#include "Character.h"

#include <memory>
#include <queue>
#include <functional>
#include <Vector3.h>
#include <Framework.h>

// 前方宣言
class EnemyManager;

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

	// 障害物の取得 (EnemyManager から)
	virtual std::vector<Vector3> GetObstacles() const;

	// EnemyManager の参照をセット
	void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }


protected:

	// 進行方向の補間処理
	Vector3 InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate);

	// 障害物回避用パス計算
	std::vector<Vector3> CalculatePath(const Vector3& start, const Vector3& goal, const std::vector<Vector3>& obstacles);

	// 障害物の近傍ノードを取得
	std::vector<Vector3> GetNeighbors(const Vector3& node);

	// ヒューリスティック関数
	float Heuristic(const Vector3& a, const Vector3& b);

protected:

    // 移動関連
	Vector3 moveVelocity_ = {};
    float moveSpeed_ = 0.0f;

    // プレイヤー関連
	Vector3 playerPosition_ = {};
	Vector3 toPlayer_ = {};

    // フレームレート補助
    static constexpr float kDefaultFrameRate = 60.0f;

	// エネミーマネージャー
	EnemyManager* enemyManager_ = nullptr;

};

