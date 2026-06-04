#include "NormalEnemy.h"

#include <Ease.h>

// BehaviorState
#include "behaviorState/normalEnemyState/EnemyBehaviorSpawn.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorMove.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorAttack.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorHitReact.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorDead.h"

#include "TimeManager.h"

using namespace IIEngine;

void NormalEnemy::Initialize()
{
    // Initialize を呼び出し共通プロパティを初期化
    BaseEnemy::Initialize("normalEnemy.obj", "NormalEnemy", initialHP_);

    SyncObjectTransform();

    // ライト設定
    object_->SetLighting(true);

    // ステートの設定
    ChangeBehaviorState(std::make_unique<EnemyBehaviorSpawn>(this));

    // スプライトの初期化
    for (uint32_t i = 0; i < spriteNum_; ++i)
    {
        auto sprite = std::make_unique<Sprite>();

        if (i == 0)
        {
            sprite->Initialize("white.png", { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.5f });
            sprite->SetSize(hpBarSize_);
        }

        sprites_.push_back(std::move(sprite));
    }

    // 行動ステート
    ChangeBehaviorState(std::make_unique<EnemyBehaviorSpawn>(this));

    // 出現時は無敵状態
    isInvincible_ = true;


    // パーティクル
    IIEngine::ParticleEmitter::Emit("laserGroup", position_, spawnParticleCount_);

    vignetteTime_ = maxVignetteTimeFrames_;
}

void NormalEnemy::Finalize()
{
    for (auto& sprite : sprites_)
    {
        sprite.reset();
    }

    for (auto& bullet : pBullets_)
    {
        bullet->SetIsDead(true);
        bullet->Finalize();
    }

    // 弾の削除
    RemoveDeadBullets<EnemyBullet>(pBullets_);

    colliderManager_->DeleteCollider(&collider_);
}

void NormalEnemy::Update()
{
    // 基底クラスの共通更新処理を呼び出し
    BaseEnemy::Update();

    // 各行動ステートの更新
    pBehaviorState_->Update();

    // モデル変形の更新
    object_->Update();

    // HPバー更新
    UpdateHPBar();

    // プレイヤーとの距離を計算
    float distanceToPlayer = position_.Distance(playerPosition_);

    if (distanceToPlayer <= kStopChasingDistance)
    {
        isFarFromPlayer_ = true;
    } 
    else
    {
        isFarFromPlayer_ = false;
    }

    // 弾の削除
    RemoveDeadBullets<EnemyBullet>(pBullets_);

    // 弾の更新
    for (auto& bullet : pBullets_)
    {
        bullet->Update();
    }

    // 暗闇処理(エネミーは動かなくさせる)
    HitVignetteTrap();


    if (!isFollowingPath_)
    {
        // プレイヤー位置が一定以上変化したらパスを再計算するフラグを立てる
        if ((playerPosition_ - lastPlayerPos_).Length() > playerMoveRePathThreshold_)
        {
            pathDirty_ = true;
            lastPlayerPos_ = playerPosition_;
        }
    }
}

void NormalEnemy::Draw()
{
    Character::Draw();

    // 弾描画
    for (auto& bullet : pBullets_)
    {
        bullet->Draw();
    }
}

void NormalEnemy::ImGuiDraw()
{
#ifdef USE_IMGUI

    ImGui::Begin("Enemy");

    ImGui::SliderFloat3("position", &position_.x, -30.0f, 30.0f);
    ImGui::SliderFloat3("rotation", &rotation_.x, -3.14f, 3.14f);
    ImGui::SliderFloat3("scale", &scale_.x, 0.0f, 10.0f);

    // オブジェクトのトランスフォーム  
    Vector3 objPos = object_->GetPosition();
    Vector3 objRot = object_->GetRotate();
    Vector3 objScale = object_->GetScale();

    if (ImGui::SliderFloat3("object position", &objPos.x, -30.0f, 30.0f)) {
        object_->SetPosition(objPos);
    }
    if (ImGui::SliderFloat3("object rotation", &objRot.x, -3.14f, 3.14f)) {
        object_->SetRotate(objRot);
    }
    if (ImGui::SliderFloat3("object scale", &objScale.x, 0.0f, 10.0f)) {
        object_->SetScale(objScale);
    }

    // HP
    ImGui::SliderFloat("HP", &hp_, 0.0f, maxHP_);

    // 現在のMoveMode
    const char* moveModeStr = (moveMode_ == MoveMode::Direct) ? "Direct" : "FollowPath";
    ImGui::Text("MoveMode: %s", moveModeStr);

    ImGui::Text("LOS True Frame: %d", losTrueFrame_);
    ImGui::Text("LOS False Frame: %d", losFalseFrame_);
    ImGui::Text("Stuck Frame: %d", stuckFrame_);
    ImGui::Text("Path Size: %zu", path_.size());
    ImGui::Text("Current Waypoint: %zu", currentWaypointIndex_);

    ImGui::End();

    for (auto& bullet : pBullets_)
    {
        bullet->ImGuiDraw();
    }

#endif // USE_IMGUI
}

void NormalEnemy::Move()
{
    toPlayer_ = playerPosition_ - position_;

    if (isFarFromPlayer_)
    {
        moveVelocity_ = { 0.0f, 0.0f, 0.0f };
        return;
    }

    obstacleGrid_.clear();

    for (const auto& o : obstaclePositions_)
    {
        GridPos g = ToGrid(o);
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dz = -1; dz <= 1; dz++)
            {
                obstacleGrid_.insert({ g.x + dx, g.z + dz });
            }
        }
    }

    // LOS判定
    GridPos enemyGrid = ToGrid(position_);
    GridPos playerGrid = ToGrid(playerPosition_);
    bool hasLOS = HasLineOfSight(enemyGrid, playerGrid);

	// LOSのフレームカウンター更新
    if (hasLOS)
    {
        losTrueFrame_++;
        losFalseFrame_ = 0;
    } 
    else
    {
        losFalseFrame_++;
        losTrueFrame_ = 0;
    }
  

    if (moveMode_ == MoveMode::FollowPath)
    {
        float distToPlayer = toPlayer_.Length();

        if (distToPlayer < followPathToDirectDistance_)
        {
            moveMode_ = MoveMode::Direct;
            path_.clear();
        }

        // ゴールが一定距離以上ズレたときだけ再計算
        float goalDist = (path_.empty()) ? 999.0f : (playerPosition_ - path_.back()).Length();

		// パスが汚れているか、ゴールが大きくズレているときは再計算
        if (pathDirty_ && goalDist > goalRePathDistance_)
        {
            path_ = CalculatePath(position_, playerPosition_, obstaclePositions_);
            currentWaypointIndex_ = 0;
            pathDirty_ = false;
        }

        // 直進復帰判定
        if (losTrueFrame_ > losTrueToDirectFrames_ && currentWaypointIndex_ > path_.size() * directReturnPathProgress_)
        {
            // 壁が無いなら直進優先
            moveMode_ = MoveMode::Direct;
            path_.clear();

            losTrueFrame_ = 0;
            losFalseFrame_ = 0;
        }
    } 
    else
    {
        // LOS判定またはスタック判定でパス計算を開始
        bool shouldCalculatePath = false;

        // LOSが無い
        if (losFalseFrame_ > losFalseToPathFrames_)
        {
            shouldCalculatePath = true;
        }

        // スタックしている
        if (stuckFrame_ > stuckToPathFrames_)
        {
            shouldCalculatePath = true;
        }

		// パス計算を開始する条件を満たしていて、かつ障害物が存在する場合にのみパス計算を行う
        if (shouldCalculatePath && !obstaclePositions_.empty())
        {
            path_ = CalculatePath(position_, playerPosition_, obstaclePositions_);
            currentWaypointIndex_ = 0;

            // パスが見つかった場合のみFollowPathに切り替え
            if (!path_.empty())
            {
                moveMode_ = MoveMode::FollowPath;
                losTrueFrame_ = 0;
                losFalseFrame_ = 0;
                stuckFrame_ = 0;
            }
        }
    } 

    Vector3 direction = { 0,0,0 };

    // パス追従
    if (moveMode_ == MoveMode::FollowPath && currentWaypointIndex_ < path_.size())
    {
        Vector3 target = path_[currentWaypointIndex_];
        Vector3 toTarget = target - position_;

        // ウェイポイント到達 
        if (toTarget.Length() < waypointArriveDist_)
        {
            currentWaypointIndex_++;
        }

        if (currentWaypointIndex_ < path_.size())
        {
            direction = Normalize(path_[currentWaypointIndex_] - position_);
        }
        else
        {
            // パス完走
            path_.clear();
            moveMode_ = MoveMode::Direct;
        }
    }

    // 直進 
    if (moveMode_ == MoveMode::Direct && direction.Length() < directionEpsilon_)
    {
        if (toPlayer_.Length() > directionEpsilon_)
        {
            direction = Normalize(toPlayer_);
        }
    }

    // 方向が取れたときだけ更新
    if (direction.Length() > directionEpsilon_)
    {
        moveVelocity_ = InterpolateMovement(moveVelocity_, direction, moveInterpolateRate_);

    } 
    else
    {
        // 行き先が無いときは少し減速
        moveVelocity_ *= noDirectionDamping_;
    }

    if (stuckFrame_ > stuckToPathFrames_ && moveMode_ == MoveMode::FollowPath)
    {
        Vector3 side = Cross({ 0,1,0 }, moveVelocity_);

        if (side.Length() > stuckSideEpsilon_)
        {
            moveVelocity_ += Normalize(side) * stuckSidePush_;
        }
    }


    moveVelocity_.y = 0.0f;

    BaseEnemy::Move();
    SyncObjectTransform();
    IIEngine::ParticleEmitter::Emit("enemyWalk", position_, walkParticleCount_);

    // スタックしていたら更新
    if ((position_ - lastPosition_).Length() < stuckDetectMoveEpsilon_)
    {
        stuckFrame_++;
    } 
    else
    {
        stuckFrame_ = 0;
    }

    lastPosition_ = position_;

    // FollowPath 用の詰まり対処
    if (stuckFrame_ > stuckFollowSkipWaypointFrames_ && moveMode_ == MoveMode::FollowPath)
    {
        currentWaypointIndex_++;
        stuckFrame_ = 0;
    }
}

void NormalEnemy::Attack()
{
    // 弾の発射設定
    const int bulletCount = attackBulletCount_;
    const float angleStep = 360.0f / bulletCount;
    const float bulletCooldown = attackCooldownSec_;

    // クールタイム管理
    static float cooldownTimer = 0.0f;
    const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

    cooldownTimer -= dt;

    // 弾の発射
    if (cooldownTimer <= 0.0f)
    {
        for (int i = 0; i < bulletCount; ++i)
        {
            // 角度計算
            float angle = DirectX::XMConvertToRadians(i * angleStep);

            // 弾の方向ベクトル計算
            Vector3 bulletDirection =
            {
                std::cos(angle), // X
                0.0f,            // Y
                std::sin(angle)  // Z
            };

            // 弾の生成と初期化
            auto bullet = std::make_unique<EnemyBullet>();
            bullet->Initialize();
            bullet->SetPosition({ position_.x, position_.y + bulletSpawnYOffset_, position_.z });
            bullet->SetVelocity(bulletDirection * bulletSpeed_);
            bullet->SyncObjectTransform();

            // 弾をリストに追加
            pBullets_.push_back(std::move(bullet));
        }

        // クールタイマーリセット
        cooldownTimer = bulletCooldown;
    }
}


void NormalEnemy::ChangeBehaviorState(std::unique_ptr<EnemyBehaviorState>&& _pState)
{
    pBehaviorState_ = std::move(_pState);
    pBehaviorState_->Initialize();
}

void NormalEnemy::OnCollisionTrigger(const Collider* _other)
{
    if (_other->GetColliderID() == "PlayerBullet" && !isInvincible_)
    {
        // プレイヤーの弾と衝突した場合
        if (hp_ > 0)
        {
            // HP減少
            hp_--;

            isHit_ = true;
        }
    }

    if (!isInvincible_ && _other->GetColliderID() == "ExplosionTimeBomb")
    {
        if (_other->GetOwner()->IsActive())
        {
            // プレイヤーのHPを減少
            if (hp_ > 0)
            {
                hp_ -= timeBombExplosionDamage_;
                isHit_ = true;
            }
        }
    }

    if (_other->GetColliderID() == "VignetteTrap")
    {
        if (_other->GetOwner()->IsActive())
        {
            // VignetteTrapに当たった場合
            isHitVignetteTrap_ = true;
        }
    }
}

void NormalEnemy::OnCollision(const Collider* _other)
{
    if (_other->GetColliderID() == "NormalEnemy" or
        _other->GetColliderID() == "TrapEnemy")
    {

        // 敵の位置
        Vector3 enemyPosition = _other->GetOwner()->GetPosition();

        // 敵同士が重ならないようにする
        Vector3 direction = position_ - enemyPosition;
        direction.Normalize();
        float distance = 2.5f; // 敵同士の間の距離を調整するための値

        // 互いに重ならないように少しずつ位置を調整
        if ((position_ - enemyPosition).Length() < distance)
        {
            position_ += direction * 0.1f; // 微調整のための値
        }
    }

    if (_other->GetColliderID() == "Wall" or
        _other->GetColliderID() == "Barrier" or
        _other->GetColliderID() == "Player")
    {
        // 相手のAABBを取得
        const AABB* otherAABB = _other->GetAABB();

        if (otherAABB)
        {
            // 自分のAABBと位置を渡して補正
            CorrectOverlap(*otherAABB, aabb_, position_);

            pathDirty_ = true;

        }
    }
}

void NormalEnemy::HitVignetteTrap()
{
    // 暗闇トラップに当たったら
    if (isHitVignetteTrap_)
    {
        // パーティクルを生成
        IIEngine::ParticleEmitter::Emit("vignetteGroup", { position_.x, position_.y + 1.0f,position_.z }, vignetteParticleCount_);

        // タイマー更新
        if (vignetteTime_ > 0)
        {
            vignetteTime_--;
        } 
        else
        {
            // 終了
            isHitVignetteTrap_ = false;
            // タイマーをリセット
            vignetteTime_ = kMaxVignetteTime;


        }
    }

}
