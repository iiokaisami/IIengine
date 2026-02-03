#include "BaseEnemy.h"

#include <unordered_map>
#include <cmath>
#include <limits>

#include "TimeManager.h"
#include "CameraManager.h"
#include "MyMath.h"
#include "Vector3Hash.h"

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
    // 位置更新
    position_ += moveVelocity_ * moveSpeed_ * dt * kDefaultFrameRate;

    // 向きは「現在の速度ベクトル」から決める
    if (moveVelocity_.Length() > 0.05f)
    {
        float targetYaw = std::atan2(moveVelocity_.x, moveVelocity_.z);
        rotation_.y = LerpAngle(rotation_.y, targetYaw, 0.2f);
    }

}

Vector3 BaseEnemy::InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate)
{
    if (currentVelocity.Length() < 0.001f)
    {
        // 今は止まっている → そのまま目標方向を返す
        return targetDirection;
    }

    if (targetDirection.Length() < 0.001f)
    {
        // 行き先不明 → 現在速度を維持
        return Normalize(currentVelocity);
    }

    Vector3 result = Slerp(currentVelocity, targetDirection, interpolationRate);

    if (result.Length() < 0.001f)
    {
        return targetDirection;
    }

    return Normalize(result);
}

std::vector<Vector3> BaseEnemy::CalculatePath(const Vector3& start, const Vector3& goal, const std::vector<Vector3>& obstacles)
{
	// ループ回数制限
    int loopCount = 0;
    const int kMaxLoop = 1000;

    // 優先度付きキュー
    struct Node
    {
        GridPos pos;
        float cost;
        bool operator>(const Node& other) const { return cost > other.cost; }
    };

	// 探索済みノードセットをクリア
    closedSet_.clear();

	// スタートとゴールをグリッド座標に変換
    GridPos startGrid = ToGrid(start);
    GridPos goalGrid = ToGrid(goal);

	// オープンセット
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;

	// コストマップと経路マップ
    std::unordered_map<GridPos, float, GridPosHash> gScore;
    std::unordered_map<GridPos, GridPos, GridPosHash> cameFrom;
    std::unordered_set<GridPos, GridPosHash> obstacleGrid;

    for (const auto& o : obstacles)
    {
        GridPos g = ToGrid(o);
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dz = -1; dz <= 1; dz++)
            {
                obstacleGrid.insert({ g.x + dx, g.z + dz });
            }
        }
    }

	// スタートノードをオープンセットに追加
    gScore[startGrid] = 0.0f;
    openSet.push({ startGrid, 0.0f });

	// メインループ
    while (!openSet.empty())
    {
        if (++loopCount > kMaxLoop)
        {
            // 見つからなかった場合は諦める
            break;
        }

		// コストの最小ノードを取得
        Node current = openSet.top();
        openSet.pop();

		// 既に探索済みならスキップ
        if (closedSet_.contains(current.pos))
        {
            continue;
        }
		// 探索済みノードに追加
        closedSet_.insert(current.pos);

		// ゴール判定
        if (current.pos == goalGrid)
        {
            std::vector<Vector3> path;
            GridPos node = goalGrid;
			// 経路を逆順にたどる
            while (cameFrom.contains(node))
            {
                path.push_back(ToWorld(node));
                node = cameFrom[node];
            }
			// スタートノードを追加
            std::reverse(path.begin(), path.end());
            return path;
        }

		// 近傍ノードを取得
        for (const Vector3& neighborWorld : GetNeighborsWorld(current.pos))
        {
			// 障害物ノードはスキップ
            if (std::find(obstacles.begin(), obstacles.end(), neighborWorld) != obstacles.end())
            {
                continue;
            }
			// グリッド座標に変換
            GridPos neighbor = ToGrid(neighborWorld);

			float tentativeGScore = gScore[current.pos] + Heuristic(ToWorld(current.pos), neighborWorld);

			// 障害物ノードはスキップ
            if (obstacleGrid.contains(neighbor))
            {
                continue;
            }

			// より良いコストなら更新
            if (!gScore.contains(neighbor) or tentativeGScore < gScore[neighbor])
            {
                gScore[neighbor] = tentativeGScore;
                cameFrom[neighbor] = current.pos;

                float fScore = tentativeGScore + Heuristic(neighborWorld, ToWorld(goalGrid));

                openSet.push({ neighbor, fScore });
            }
        }
    }

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

BaseEnemy::GridPos BaseEnemy::ToGrid(const Vector3& v) const
{
    return 
    {
         static_cast<int>(std::round(v.x / 1.0f)),
         static_cast<int>(std::round(v.z / 1.0f))
    };
}

Vector3 BaseEnemy::ToWorld(const GridPos& g) const
{
    return Vector3(static_cast<float>(g.x + 0.5f) * 1.0f, 0.0f, static_cast<float>(g.z + 0.5f) * 1.0f);
}

std::vector<Vector3> BaseEnemy::GetNeighborsWorld(const GridPos& node)
{
    std::vector<GridPos> neighbors =
    {
        { node.x + 1, node.z },
        { node.x - 1, node.z },
        { node.x, node.z + 1 },
        { node.x, node.z - 1 }
    };

    std::vector<Vector3> result;
    for (const auto& n : neighbors)
    {
        result.push_back(ToWorld(n));
    }
    return result;
}
