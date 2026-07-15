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

    moveVelocity_ = moveVelocityDefault_;
    moveSpeed_ = moveSpeedDefault_;

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
    if (moveVelocity_.Length() > moveFacingMinVelocity_)
    {
        float targetYaw = std::atan2(moveVelocity_.x, moveVelocity_.z);
        rotation_.y = LerpAngle(rotation_.y, targetYaw, 0.2f);
    }

}

void BaseEnemy::SeparateFromEnemy(const IIEngine::Collider* _other, float _Distance)
{
    Vector3 enemyPosition = _other->GetOwner()->GetPosition();

    Vector3 direction = position_ - enemyPosition;
    direction.Normalize();

    if ((position_ - enemyPosition).Length() < _Distance)
    {
        position_ += direction * 0.1f;
    }
}

void BaseEnemy::ResolveWallCollision(const IIEngine::Collider* _other)
{
    // 相手のAABBを取得
    const IIEngine::AABB* otherAABB = _other->GetAABB();

    if (otherAABB)
    {
        // 自分のAABBと位置を渡して補正
        CorrectOverlap(*otherAABB, aabb_, position_);

        pathDirty_ = true;

    }
}

void BaseEnemy::ReceiveBulletDamage()
{
    // 弾と衝突した場合
    if (hp_ > 0.0f)
    {
        // HP減少
        hp_--;

    }

}

Vector3 BaseEnemy::InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate)
{
    if (currentVelocity.Length() < directionEpsilon_)
    {
        // 今は止まっている → そのまま目標方向を返す
        return targetDirection;
    }

    if (targetDirection.Length() < directionEpsilon_)
    {
        // 行き先不明 → 現在速度を維持
        return Normalize(currentVelocity);
    }

    Vector3 result = Slerp(currentVelocity, targetDirection, interpolationRate);

    if (result.Length() < directionEpsilon_)
    {
        return targetDirection;
    }

    return Normalize(result);
}

std::vector<Vector3> BaseEnemy::CalculatePath(const Vector3& start, const Vector3& goal, const std::vector<Vector3>& obstacles)
{
    // ループ回数制限
    int loopCount = 0;

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

    // スタートノードをオープンセットに追加
    gScore[startGrid] = 0.0f;
    openSet.push({ startGrid, 0.0f });

    // メインループ
    while (!openSet.empty())
    {
        if (++loopCount > maxPathSearchLoop_)
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
            
			// パスをスムージング
            return SmoothPath(path);
           
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
            if (obstacleGrid_.contains(neighbor))
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
        { node.x, node.z - 1 },

        { node.x + 1, node.z + 1 },
        { node.x + 1, node.z - 1 },
        { node.x - 1, node.z + 1 },
        { node.x - 1, node.z - 1 }
    };

    std::vector<Vector3> result;
    for (const auto& n : neighbors)
    {
        int dx = n.x - node.x;
        int dz = n.z - node.z;

        // corner cutting防止
        if (dx != 0 && dz != 0)
        {
            // 横か縦のどちらかが壁なら斜め禁止
            if (obstacleGrid_.contains({ node.x + dx, node.z }) ||
                obstacleGrid_.contains({ node.x, node.z + dz }))
            {
                continue;
            }
        }

        result.push_back(ToWorld(n));
    }
    return result;
}

bool BaseEnemy::HasLineOfSight(const GridPos& start, const GridPos& end)
{
	// アルゴリズムでスタートからエンドまでのセルを列挙し、障害物がないかチェック
    int x0 = start.x;
    int z0 = start.z;
    int x1 = end.x;
    int z1 = end.z;

    int dx = abs(x1 - x0);
    int dz = abs(z1 - z0);

    int sx = (x0 < x1) ? 1 : -1;
    int sz = (z0 < z1) ? 1 : -1;

    int err = dx - dz;

	// スタートからエンドまでのセルを列挙
    while (true)
    {
        // エンドに到達したら成功
        if (x0 == x1 && z0 == z1)
        {
            return true;
        }

        // 現在のセルとその周辺に障害物があるかチェック
        if (obstacleGrid_.contains({ x0, z0 }))
        {
            return false;
        }

        int e2 = 2 * err;

        // 斜め移動のときは両方のセルをチェックする必要があるため、先にエラーを計算してから移動する
        if (e2 > -dz)
        {
            err -= dz;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            z0 += sz;
        }
    }

    return true;
}

std::vector<Vector3> BaseEnemy::SmoothPath(const std::vector<Vector3>& path)
{
    if (path.size() < 3)
    {
        return path; // ウェイポイント数が少ない場合はスムージング不要
    }

    std::vector<Vector3> smoothedPath;
    smoothedPath.push_back(path.front()); // スタート地点を追加

    size_t i = 0;
    while (i < path.size() - 1)
    {
        // 現在地から見える最も遠いウェイポイントを探す
        size_t farthest = i + 1;

        for (size_t j = path.size() - 1; j > i + 1; --j)
        {
            GridPos start = ToGrid(path[i]);
            GridPos end = ToGrid(path[j]);

            if (HasLineOfSight(start, end))
            {
                farthest = j;
                break;
            }
        }

        smoothedPath.push_back(path[farthest]);
        i = farthest;
    }

    return smoothedPath;
}
