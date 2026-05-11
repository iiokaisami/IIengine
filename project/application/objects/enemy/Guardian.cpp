#include "Guardian.h"

// BehaviorState
#include "behaviorState/guardianState/GuardianBehaviorSpawn.h"
#include "behaviorState/guardianState/GuardianBehaviorMove.h"
#include "behaviorState/guardianState/GuardianBehaviorAttack.h"
#include "behaviorState/guardianState/GuardianBehaviorHitReact.h"
#include "behaviorState/guardianState/GuardianBehaviorDead.h"

#include "TimeManager.h"

using namespace IIEngine;

void Guardian::Initialize()
{
    // Initialize を呼び出し共通プロパティを初期化
    BaseEnemy::Initialize("guardian.obj", "guardian", initialHP_);

    SyncObjectTransform();

    // ライト設定
    object_->SetLighting(true);

    // 出現時は無敵状態
    isInvincible_ = true;

    // パーティクル
    IIEngine::ParticleEmitter::Emit("laserGroup", position_, spawnParticleCount_);


    //vignetteTime_ = maxVignetteTimeFrames_;
}

void Guardian::Finalize()
{
    colliderManager_->DeleteCollider(&collider_);
}

void Guardian::Update()
{
    // 基底クラスの共通更新処理を呼び出し
    BaseEnemy::Update();

    // モデル変形の更新
    object_->Update();

    // プレイヤーとの距離を計算
    float distanceToPlayer = position_.Distance(playerPosition_);

    if (distanceToPlayer <= kStopChasingDistance)
    {
        isFarFromPlayer_ = true;
    } else
    {
        isFarFromPlayer_ = false;
    }

    Move();

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

void Guardian::Draw()
{
    Character::Draw();

}

void Guardian::ImGuiDraw()
{
#ifdef USE_IMGUI
    
    ImGui::Begin("Guardian");
    ImGui::Text("HP: %.1f", hp_);
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", position_.x, position_.y, position_.z);
    ImGui::Text("Is Invincible: %s", isInvincible_ ? "Yes" : "No");
    ImGui::Text("Is Following Path: %s", isFollowingPath_ ? "Yes" : "No");
    ImGui::Text("Is Far From Player: %s", isFarFromPlayer_ ? "Yes" : "No");
	ImGui::End();

#endif // USE_IMGUI

}

void Guardian::Move()
{
    // デルタタイム取得
    const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

    // 基底クラスの補間処理を使用
    toPlayer_ = playerPosition_ - position_;
    Vector3 direction = Normalize(toPlayer_);
    moveVelocity_ = InterpolateMovement(moveVelocity_, direction, moveInterpolateRate_);

    // Y軸回転を進行方向に合わせる
    rotation_.y = std::atan2(moveVelocity_.x, moveVelocity_.z);
    rotation_.x = 0.0f;

    moveVelocity_ /= moveVelocityDivisor_;
    moveVelocity_.y = 0.0f;
    position_ += moveVelocity_ * dt * kDefaultFrameRate;

    SyncObjectTransform();

    IIEngine::ParticleEmitter::Emit("enemyWalk", position_, moveParticleCount_);

}

void Guardian::ChangeBehaviorState(std::unique_ptr<GuardianBehaviorState> _pState)
{
}

void Guardian::OnCollisionTrigger(const IIEngine::Collider* _other)
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
            //isHitVignetteTrap_ = true;
        }
    }
}

void Guardian::OnCollision(const IIEngine::Collider* _other)
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
