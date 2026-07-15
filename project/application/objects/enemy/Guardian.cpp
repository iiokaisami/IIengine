#include "Guardian.h"

// BehaviorState
#include "behaviorState/guardianState/GuardianBehaviorSpawn.h"
#include "behaviorState/guardianState/GuardianBehaviorMove.h"
#include "behaviorState/guardianState/GuardianBehaviorHitReact.h"
#include "behaviorState/guardianState/GuardianBehaviorDead.h"

// 弾のパターン
#include "bullet/bulletPattern/SpreadShotPattern.h"
#include "bullet/bulletPattern/SpiralShotPattern.h"
#include "bullet/bulletPattern/LaserPattern.h"

#include "TimeManager.h"

using namespace IIEngine;

void Guardian::Initialize()
{
    // Initialize を呼び出し共通プロパティを初期化
    BaseEnemy::Initialize("guardian.obj", "Guardian", initialHP_);

	position_.y = 1.0f; // 地面に立つようにY座標を調整
	scale_ = { 2.0f, 2.0f, 2.0f };

    SyncObjectTransform();

    // ライト設定
    object_->SetLighting(true);

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

    // 出現時は無敵状態
    isInvincible_ = true;

	// 行動ステートの初期化
	pBehaviorState_ = std::make_unique<GuardianBehaviorSpawn>(this);

    // パーティクル
    //IIEngine::ParticleEmitter::Emit("laserGroup", position_, spawnParticleCount_);

	// 弾幕のパターンを追加
	attackController_.AddPattern(std::make_unique<SpreadShotPattern>(spreadCount_, spreadAngle_, spreadBulletSpeed_));
	attackController_.AddPattern(std::make_unique<SpiralShotPattern>(spiralAngleSpeed_, spiralBulletSpeed_));
	attackController_.AddPattern(std::make_unique<LaserPattern>(laserDirection_, laserSpeed_));

}

void Guardian::Finalize()
{
	// 弾のリソース解放
	bulletManager_.ClearBullets();

    colliderManager_->DeleteCollider(&collider_);
}

void Guardian::Update()
{
    // 基底クラスの共通更新処理を呼び出し
    BaseEnemy::Update();

	// 行動ステートの更新
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

    Attack();

	// プレイヤーに向く
    FaceToPlayer();

	// 衝撃波の更新
	if (pShockWave_)
	{
		pShockWave_->Update();
	}
    // 衝撃波の削除
    if (pShockWave_ && pShockWave_->IsDead())
    {
		pShockWave_->Finalize();
		pShockWave_.reset();
    }

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
	// 基底クラスの描画処理を呼び出し
    Character::Draw();

	// 弾の描画
	bulletManager_.DrawAll();
	
	// 衝撃波の描画
	if (pShockWave_)
	{
		pShockWave_->Draw();
	}
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
	
    int maxPattern = static_cast<int>(attackController_.PatternCount()) - 1;
    int patternIndex = static_cast<int>(currentPatternIndex_);

    if (ImGui::SliderInt("Bullet Pattern", &patternIndex, 0, maxPattern)) {
        currentPatternIndex_ = static_cast<size_t>(patternIndex);
        attackController_.SetPatternIndex(currentPatternIndex_);
    }


    ImGui::Text("Current Pattern Index: %d", patternIndex);
    ImGui::Text("Total Patterns: %d", maxPattern + 1);
    
	// プレイヤーとの距離を表示
	float distanceToPlayer = position_.Distance(playerPosition_);
        ImGui::Text("Distance to Player: %.2f", distanceToPlayer);

	// 攻撃周期を表示
	ImGui::Text("Attack Cycle: %.2f", attackCycle_);
  
    // ジャンプ開始
	if (ImGui::Button("Start Jump"))
	{
		isJumping_ = true;
	}


	// 衝撃波のImGui表示
    if (pShockWave_)
    {
		pShockWave_->ImGuiDraw();
    }

    ImGui::End();

#endif // USE_IMGUI

}

void Guardian::Move()
{
    // デルタタイム取得
    const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// プレイヤーとの距離を計算
    float distanceToPlayer = (playerPosition_ - position_).Length();

	// プレイヤーに近づきすぎないようにする
    if (distanceToPlayer <= kStopChasingDistance)
    {
        moveVelocity_ = { 0.0f, 0.0f, 0.0f };
        // ここでposition_も変えずにSyncObjectTransform()だけでOK
        SyncObjectTransform();
        // 歩きパーティクルも止めるならemitしない
        return;
    }

    // 基底クラスの補間処理を使用
    toPlayer_ = playerPosition_ - position_;
    Vector3 direction = Normalize(toPlayer_);
    moveVelocity_ = InterpolateMovement(moveVelocity_, direction, moveInterpolateRate_);

    moveVelocity_ /= moveVelocityDivisor_;
    moveVelocity_.y = 0.0f;
    position_ += moveVelocity_ * dt * kDefaultFrameRate;

    SyncObjectTransform();

    IIEngine::ParticleEmitter::Emit("enemyWalk", position_, moveParticleCount_);

}

void Guardian::Attack()
{
	float now = IIEngine::TimeManager::Instance().GetTotalTime();
    float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

    std::vector<PatternCondition> patternConditions_ = 
    {
		// 条件とパターンインデックスのペアを定義
        // 0: SpreadShotPattern
        { [](const Guardian& g) { return g.GetDistanceToPlayer() < g.kSpreadRange_ && g.GetDistanceToPlayer() < g.kSpiralRange_; }, 0 },
        // 1: SpiralShotPattern
        { [](const Guardian& g) { return g.GetDistanceToPlayer() >= g.kSpiralRange_ && g.GetDistanceToPlayer() < g.kLaserRange_; }, 1 },
        // 2: LaserPattern
        { [](const Guardian& g) { return g.GetDistanceToPlayer() >= g.kLaserRange_; }, 2 },
        
    };

	// 条件を順番に評価して、最初にマッチしたパターンに切り替える
    for (const auto& entry : patternConditions_) 
    {
		// 条件を評価
        if (entry.condition(*this)) 
        {
			// 条件にマッチしたパターンに切り替える(すでにそのパターンの場合は何もしない)
            if (currentPatternIndex_ != entry.patternIndex) 
            {
				// パターン切り替え
                currentPatternIndex_ = entry.patternIndex;
                attackController_.SetPatternIndex(currentPatternIndex_);
            }

            break;
        }
    }

    // SpiralShotPatternのインデックス(登録順)
    uint32_t spiralIndex = 1; 
    // 現在のパターンがSpiralShotPatternかどうか
    bool isSpiral = (currentPatternIndex_ == spiralIndex);

    // 発射間隔切り替え
    if (isSpiral)
    {
		// SpiralShotPatternの発射間隔
        shootInterval_ = 1.0f / 6.0f;
    } 
    else 
    {
        shootInterval_ = 3.0f; // 通常時
    }

    // 発射間隔管理
    shootTimer_ += dt;
    if (shootTimer_ >= shootInterval_ && !isJumping_)
    {
		// 現在のパターンで弾を発射
        attackController_.FireCurrentPattern(position_, bulletManager_, now, playerPosition_);
        shootTimer_ = 0.0f;

		// SpiralShotPatternの場合は撃った回数をカウント
        if (isSpiral)
        {
            ++spiralShotsFired_;
            // spiralMaxShots_分撃ち終わったら次へ
            if (spiralShotsFired_ >= spiralMaxShots_) 
            {
				// 次のパターンへ切り替え
                currentPatternIndex_ = (currentPatternIndex_ + 1) % attackController_.PatternCount();
                attackController_.SetPatternIndex(currentPatternIndex_);
                spiralShotsFired_ = 0;
                patternTimer_ = 0.0f; // パターン切り替え直後から再カウント

                // 攻撃カウントを増加
                ++attackCycle_;
            }
        }
        else
        {
            // Spiral以外は従来通りパターンタイマーで管理
            patternTimer_ += shootInterval_; 
            if (patternTimer_ >= patternInterval_) 
            {
                currentPatternIndex_ = (currentPatternIndex_ + 1) % attackController_.PatternCount();
                attackController_.SetPatternIndex(currentPatternIndex_);
                patternTimer_ = 0.0f;

                // 攻撃カウントを増加
                ++attackCycle_;
            }
        }
    }
	bulletManager_.UpdateAll();

}

void Guardian::StartJump()
{
	// ジャンプ状態を上昇に設定
    jumpState_ = JumpState::Rise;

	// ジャンプ開始位置を記録
    jumpStartY_ = position_.y;

	// 着地フラグをリセット
    isLanding_ = false;

	// 攻撃周期をリセット
	attackCycle_ = 0.0f;

}

void Guardian::JumpAttack()
{
	// ジャンプ状態に応じて処理を分岐
    switch (jumpState_)
    {
    case JumpState::None:
        break;

    case JumpState::Rise:
		// 上昇中の処理
        UpdateJumpRise();
        break;

    case JumpState::Fall:
		// 下降中の処理
        UpdateJumpFall();
        break;

    case JumpState::Landing:
		// 着地中の処理
        UpdateJumpLanding();
        break;
    }


}

void Guardian::UpdateJumpRise()
{
    // deltaTime取得
    float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// 上昇速度を加算
    position_.y += ascendSpeed_ * dt;

	// 最大高度に達したら下降状態に切り替え
    if (position_.y >= jumpStartY_ + jumpMaxHeight_)
    {
		// 最大高度に達したら位置を補正
        position_.y = jumpStartY_ + jumpMaxHeight_;
		// 下降状態に切り替え
        jumpState_ = JumpState::Fall;
    }

}

void Guardian::UpdateJumpFall()
{
    // deltaTime取得
    float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// 下降速度を加算
    position_.y -= descendSpeed_ * dt;

	// 地面に到達したら位置を補正して着地状態に切り替え
    if (position_.y <= jumpStartY_)
    {
		// 地面に到達したら位置を補正
        position_.y = jumpStartY_;

		// 着地状態に切り替え
        jumpState_ = JumpState::Landing;
    }
}

void Guardian::UpdateJumpLanding()
{
	// 衝撃波を生成
    SpawnShockWave();

	// 着地パーティクルを生成
    ParticleEmitter::Emit("enemyWalk",position_,50);

	// 着地フラグを立てる
	isLanding_ = true;

    jumpState_ = JumpState::None;
}

void Guardian::SpawnShockWave()
{
	pShockWave_ = std::make_unique<ShockWave>();
	pShockWave_->Initialize();
	pShockWave_->SetPosition(position_);
}

void Guardian::ChangeBehaviorState(std::unique_ptr<GuardianBehaviorState> _pState)
{
    pBehaviorState_ = std::move(_pState);
    pBehaviorState_->Initialize();
}

void Guardian::FaceToPlayer()
{
    Vector3 toPlayer = playerPosition_ - position_;

    rotation_.x = 0.0f;
    rotation_.y = std::atan2(toPlayer.x, toPlayer.z); 

	object_->SetRotate(rotation_);
}

void Guardian::SwitchAttackPattern(uint32_t _patternIndex)
{
    if (currentPatternIndex_ != _patternIndex) 
    {
        currentPatternIndex_ = _patternIndex;
        attackController_.SetPatternIndex(currentPatternIndex_);
        spiralShotsFired_ = 0;
        patternTimer_ = 0.0f;
    }
}

void Guardian::OnCollisionTrigger(const IIEngine::Collider* _other)
{
    if (_other->GetColliderID() == "PlayerBullet" && !isInvincible_)
    {
        // 弾の衝突時にダメージを受ける処理
        ReceiveBulletDamage(_other);
    }

    if (!isInvincible_ && _other->GetColliderID() == "ExplosionTimeBomb")
    {
        if (_other->GetOwner()->IsActive())
        {
            // HPを減少
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
        }
    }
}

void Guardian::OnCollision(const IIEngine::Collider* _other)
{
    if (_other->GetColliderID() == "NormalEnemy" or
        _other->GetColliderID() == "TrapEnemy")
    {

        // 敵同士の衝突時に距離を保つように押し出す処理
        SeparateFromEnemy(_other, 2.5f);
    }

    if (_other->GetColliderID() == "Wall" or
        _other->GetColliderID() == "Barrier" or
        _other->GetColliderID() == "Player")
    {
        // 衝突時に押し出す処理
        ResolveWallCollision(_other);
    }
}
