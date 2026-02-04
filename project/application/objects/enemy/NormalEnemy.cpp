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
    BaseEnemy::Initialize("normalEnemy.obj", "NormalEnemy", 3.0f);

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
    IIEngine::ParticleEmitter::Emit("laserGroup", position_, 2);
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
    } else
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
        if ((playerPosition_ - lastPlayerPos_).Length() > 1.0f)
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

    if (!obstaclePositions_.empty())
    {
        if (path_.empty() or pathDirty_)
        {
            path_ = CalculatePath(position_, playerPosition_, obstaclePositions_);
            currentWaypointIndex_ = 0;
            moveMode_ = path_.empty() ? MoveMode::Direct : MoveMode::FollowPath;
            pathDirty_ = false;
        }
    } else
    {
        moveMode_ = MoveMode::Direct;
    }

    Vector3 direction = { 0,0,0 };

    // パス追従
    if (moveMode_ == MoveMode::FollowPath && currentWaypointIndex_ < path_.size())
    {
        Vector3 target = path_[currentWaypointIndex_];
        Vector3 toTarget = target - position_;
        Vector3 dir = Normalize(moveVelocity_);

        // ウェイポイント到達 
        if (Dot(Normalize(toTarget), dir) < 0.0f or toTarget.Length() < 1.2f)
        {
            currentWaypointIndex_++;
        }

        if (currentWaypointIndex_ < path_.size())
        {
            direction = Normalize(path_[currentWaypointIndex_] - position_);
        } else
        {
            // パス完走
            path_.clear();
            moveMode_ = MoveMode::Direct;
        }
    }

    // 直進 
    if (moveMode_ == MoveMode::Direct && direction.Length() < 0.001f)
    {
        if (toPlayer_.Length() > 0.001f)
        {
            direction = Normalize(toPlayer_);
        }
    }

    // 方向が取れたときだけ更新
    if (direction.Length() > 0.001f)
    {
        moveVelocity_ = InterpolateMovement(moveVelocity_, direction, 0.15f);

    } else
    {
        // 行き先が無いときは少し減速
        moveVelocity_ *= 0.9f;
    }

    if (stuckFrame_ > 10 && moveMode_ == MoveMode::FollowPath)
    {
        Vector3 side = Cross({ 0,1,0 }, moveVelocity_);

        if (side.Length() > 0.0001f)
        {
            moveVelocity_ += Normalize(side) * 0.1f;
        }
    }


    moveVelocity_.y = 0.0f;

    BaseEnemy::Move();
    SyncObjectTransform();
    IIEngine::ParticleEmitter::Emit("enemyWalk", position_, 1);

    // スタックしていたら更新
    if ((position_ - lastPosition_).Length() < 0.01f)
    {
        stuckFrame_++;
    } else
    {
        stuckFrame_ = 0;
    }

    lastPosition_ = position_;

    // FollowPath 用の詰まり対処
    if (stuckFrame_ > 30 && moveMode_ == MoveMode::FollowPath)
    {
        currentWaypointIndex_++;
        stuckFrame_ = 0;
    }
    // Direct 用の詰まり対処
    if (stuckFrame_ > 30 && moveMode_ == MoveMode::Direct)
    {
        pathDirty_ = true;
        path_.clear();
        currentWaypointIndex_ = 0;
        stuckFrame_ = 0;
    }
}

void NormalEnemy::Attack()
{
    // 弾の発射設定
    const int bulletCount = 24;
    const float angleStep = 360.0f / bulletCount;
    const float bulletCooldown = 0.0f;

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
            bullet->SetPosition({ position_.x, position_.y + 0.5f, position_.z });
            bullet->SetVelocity(bulletDirection * 0.2f);
            bullet->SyncObjectTransform();

            // 弾をリストに追加
            pBullets_.push_back(std::move(bullet));
        }

        // クールタイマーリセット
        cooldownTimer = bulletCooldown;
    }
}


void NormalEnemy::ChangeBehaviorState(std::unique_ptr<EnemyBehaviorState> _pState)
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
                hp_ -= 1.5f;
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
        _other->GetColliderID() == "Barrie" or
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
        IIEngine::ParticleEmitter::Emit("vignetteGroup", { position_.x, position_.y + 1.0f,position_.z }, 3);

        // タイマー更新
        if (vignetteTime_ > 0)
        {
            vignetteTime_--;
        } else
        {
            // 終了
            isHitVignetteTrap_ = false;
            // タイマーをリセット
            vignetteTime_ = kMaxVignetteTime;


        }
    }

}
