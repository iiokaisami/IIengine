#include "NormalEnemy.h"

#include <Ease.h>

// BehaviorState
#include "behaviorState/normalEnemyState/EnemyBehaviorSpawn.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorMove.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorAttack.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorHitReact.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorDead.h"

#include "TimeManager.h"

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
	// 前フレームの位置
    Vector3 lastPosition_;
    int stuckFrame_ = 0;

    toPlayer_ = playerPosition_ - position_;

    if (isFarFromPlayer_)
    {
        moveVelocity_ = { 0.0f, 0.0f, 0.0f };
        return;
    }

    Vector3 direction = { 0,0,0 };

    // パスが無い & 障害物があるときだけ計算
    if (!obstaclePositions_.empty() && path_.empty())
    {
        path_ = CalculatePath(position_, playerPosition_, obstaclePositions_);
        currentWaypointIndex_ = 0;
        moveMode_ = path_.empty() ? MoveMode::Direct : MoveMode::FollowPath;
    }

    // パス追従
    if (moveMode_ == MoveMode::FollowPath && currentWaypointIndex_ < path_.size())
    {
        Vector3 target = path_[currentWaypointIndex_];
        Vector3 toTarget = target - position_;

        // ウェイポイント到達 
        if (toTarget.Length() < 1.0f)
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
        Vector3 currentDir = Normalize(moveVelocity_);
        float dot = Dot(currentDir, direction);

        // ほぼ同じ方向なら補間しない
        if (dot > 0.98f)
        {
            moveVelocity_ = direction;
        } 
        else
        {
            moveVelocity_ = InterpolateMovement(moveVelocity_, direction, 0.15f);
        }

        direction = Normalize(toPlayer_);
        moveVelocity_ = InterpolateMovement(moveVelocity_, direction, 0.15f);
    }
    moveVelocity_.y = 0.0f;

    BaseEnemy::Move();
    SyncObjectTransform();
    IIEngine::ParticleEmitter::Emit("enemyWalk", position_, 1);

    if ((position_ - lastPosition_).Length() < 0.01f)
    {
        stuckFrame_++;
    } else
    {
        stuckFrame_ = 0;
    }

    lastPosition_ = position_;

    if (stuckFrame_ > 30) // 約0.5秒
    {
        moveMode_ = MoveMode::Direct;
        path_.clear();
        currentWaypointIndex_ = 0;
        stuckFrame_ = 0;
    }


}

void NormalEnemy::Attack()
{
    // 弾の数と間隔角度
    const int bulletCount = 24;
    const float angleStep = 360.0f / bulletCount;

    for (int i = 0; i < bulletCount; ++i)
    {
        // 弾の角度を計算 (ラジアンに変換)
        float angle = DirectX::XMConvertToRadians(i * angleStep);

        // 弾の方向を計算
        Vector3 bulletDirection =
        {
            std::cos(angle), // X成分
            0.0f,            // Y成分
            std::sin(angle)  // Z成分
        };

        // 弾を生成
        auto bullet = std::make_unique<EnemyBullet>();
        bullet->Initialize();
        bullet->SetPosition({ position_.x,position_.y + 0.5f,position_.z }); // 敵の位置より少し上を初期位置に設定
        bullet->SetVelocity(bulletDirection * 0.2f);
        bullet->SyncObjectTransform();

        // 弾をリストに追加
        pBullets_.push_back(std::move(bullet));
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
