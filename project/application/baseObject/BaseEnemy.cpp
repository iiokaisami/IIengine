#include "BaseEnemy.h"

#include <unordered_map>
#include <cmath>
#include <limits>

#include "TimeManager.h"
#include "CameraManager.h"
#include "MyMath.h"
#include "../objects/enemy/EnemyManager.h"

void BaseEnemy::Initialize(const std::string& _modelFileName, const std::string& _objectName, float _initialHP)
{
	// キャラクター基底クラスの初期化を呼び出し
	Character::Initialize(_modelFileName, _objectName, _initialHP);
    
    moveVelocity_ = { 0.1f, 0.0f, 0.1f };
    moveSpeed_ = 0.05f;

}

void BaseEnemy::Update()
{
    
	Character::Update();
}

void BaseEnemy::Move() 
{
    // デルタタイム取得
    const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

    // プレイヤー方向ベクトル計算
    toPlayer_ = playerPosition_ - position_;
    moveVelocity_ = Normalize(toPlayer_);  // 基本ではプレイヤーに向かう

    // Y軸回転を進行方向に合わせる
    rotation_.y = std::atan2(moveVelocity_.x, moveVelocity_.z);

    // デフォルトの位置更新
    position_ += moveVelocity_ * moveSpeed_ * dt * kDefaultFrameRate;

}

std::vector<Vector3> BaseEnemy::GetObstacles() const
{
    if (enemyManager_)
    {
        return enemyManager_->GetObstacles();
    }
    return {};
}

Vector3 BaseEnemy::InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate)
{
    return Normalize(Slerp(currentVelocity, targetDirection, interpolationRate));
}

std::vector<Vector3> BaseEnemy::CalculatePath(const Vector3& start, const Vector3& goal, const std::vector<Vector3>& obstacles)
{
    //// 優先度付きキュー
    //struct Node 
    //{
    //    Vector3 position;
    //    float cost;
    //    bool operator>(const Node& other) const { return cost > other.cost; }
    //};

    //std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;

    //// 各ノードのコスト保存
    //std::unordered_map<Vector3, float> gScore;
    //std::unordered_map<Vector3, Vector3> cameFrom;

    //gScore[start] = 0;
    //openSet.push({ start, Heuristic(start, goal) });

    //// 探索開始
    //while (!openSet.empty())
    //{
    //    Node current = openSet.top();
    //    openSet.pop();

    //    if (current.position.x == goal.x && current.position.y == goal.y && current.position.z == goal.z)
    //    {
    //        // ゴールに到達した場合、経路を再構築
    //        std::vector<Vector3> path;
    //        for (Vector3 node = goal; cameFrom.find(node) != cameFrom.end(); node = cameFrom[node])
    //        {
    //            path.push_back(node);
    //        }
    //        std::reverse(path.begin(), path.end());
    //        return path;
    //    }

    //    // 近傍ノードを調べる
    //    for (const Vector3& neighbor : GetNeighbors(current.position))
    //    {
    //        if (std::find(obstacles.begin(), obstacles.end(), neighbor) != obstacles.end()) 
    //        {
    //            // 障害物はスキップ
    //            continue;
    //        }

    //        float tentativeGScore = gScore[current.position] + Heuristic(current.position, neighbor);
    //        if (gScore.find(neighbor) == gScore.end() or tentativeGScore < gScore[neighbor]) 
    //        {
    //            // コストが小さい場合更新
    //            gScore[neighbor] = tentativeGScore;
    //            float fScore = tentativeGScore + Heuristic(neighbor, goal);
    //            openSet.push({ neighbor, fScore });
    //            cameFrom[neighbor] = current.position;
    //        }
    //    }
    //}

    start;
	goal;
	obstacles;

    // パスが見つからない場合
    return {};
}

std::vector<Vector3> BaseEnemy::GetNeighbors(const Vector3& node)
{
    // 近傍のノードを生成(グリッド状を仮定)
    std::vector<Vector3> neighbors = 
    {
        node + Vector3(1, 0, 0),
        node + Vector3(-1, 0, 0),
        node + Vector3(0, 0, 1),
        node + Vector3(0, 0, -1),
    };
    return neighbors;
}

float BaseEnemy::Heuristic(const Vector3& a, const Vector3& b)
{
    return (a - b).Length(); // ユークリッド距離
}
