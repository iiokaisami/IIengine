#include "NormalEnemy.h"

#include <Ease.h>

// BehaviorState
#include "behaviorState/normalEnemyState/EnemyBehaviorSpawn.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorMove.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorAttack.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorHitReact.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorDead.h"

void NormalEnemy::Initialize()
{
    // --- 3Dオブジェクト ---
    object_ = std::make_unique<Object3d>();
    object_->Initialize("normalEnemy.obj");

    moveVelocity_ = { 0.1f,0.1f,0.0f };

    object_->SetPosition(position_);
    object_->SetRotate(rotation_);
    // 仮置き
    scale_ = { 1.0f,1.0f,1.0f };
	object_->SetScale(scale_);
    // ライト設定
    object_->SetDirectionalLightEnable(true);

    colliderManager_ = ColliderManager::GetInstance();

    objectName_ = "NormalEnemy";

    desc =
    {
        //ここに設定
        .owner = this,
        .colliderID = objectName_,
        .shape = Shape::AABB,
        .shapeData = &aabb_,
        .attribute = colliderManager_->GetNewAttribute(objectName_),
        .onCollision = std::bind(&NormalEnemy::OnCollision, this, std::placeholders::_1),
        .onCollisionTrigger = std::bind(&NormalEnemy::OnCollisionTrigger, this, std::placeholders::_1),
    };
    collider_.MakeAABBDesc(desc);
    colliderManager_->RegisterCollider(&collider_);

    // ステータス
    hp_ = 3;
    isDead_ = false;

    // 行動ステート
    ChangeBehaviorState(std::make_unique<EnemyBehaviorSpawn>(this));

	// 出現時は無敵状態
    isInvincible_ = true;


	// パーティクル
	ParticleEmitter::Emit("laserGroup", position_, 2);
}

void NormalEnemy::Finalize()
{
	for (auto& bullet : pBullets_)
	{
		bullet->SetIsDead(true);
		bullet->Finalize();
	}

    pBullets_.erase(
        std::remove_if(pBullets_.begin(), pBullets_.end(), [](std::unique_ptr<EnemyBullet>& bullet)
            {
                if (bullet->IsDead())
                {
                    bullet->Finalize();
                    return true;
                }
                return false;
            }),
        pBullets_.end()
    );

    colliderManager_->DeleteCollider(&collider_);
}

void NormalEnemy::Update()
{
	// 各行動ステートの更新
	pBehaviorState_->Update();

    object_->Update();

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

	// 敵の弾の削除
    pBullets_.erase(
        std::remove_if(pBullets_.begin(), pBullets_.end(), [](std::unique_ptr<EnemyBullet>& bullet)
            {
                if (bullet->IsDead())
                {
                    bullet->Finalize();
                    return true;
                }
                return false;
            }),
        pBullets_.end()
    );

    // 弾の更新
    for (auto& bullet : pBullets_)
    {
        bullet->Update();
    }

	// aabbの更新
    aabb_.min = position_ - object_->GetScale();
    aabb_.max = position_ + object_->GetScale();
    aabb_.max.y += 1.0f;
    collider_.SetPosition(position_);

	// 暗闇処理(エネミーは動かなくさせる)
    HitVignetteTrap();
}

void NormalEnemy::Draw()
{
    object_->Draw();

	// 弾描画
	for (auto& bullet : pBullets_)
	{
		bullet->Draw();
	}
}

void NormalEnemy::Draw2D()
{
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

    ImGui::End();

	for (auto& bullet : pBullets_)
	{
		bullet->ImGuiDraw();
	}

#endif // USE_IMGUI
}

void NormalEnemy::Move()
{

    if (isFarFromPlayer_)
    {
        // プレイヤーとの距離が一定以下の場合、追尾を停止(0にすると色々まずかった)
        moveVelocity_ = { 0.000001f, 0.0f, 0.0f };        
        return;
    }
    else
    {
        // 敵弾から自キャラへのベクトルを計算
        toPlayer_ = playerPosition_ - position_;

        // ベクトルを正規化する
        toPlayer_ = Normalize(toPlayer_);
        moveVelocity_ = Normalize(moveVelocity_);

        // 球面線形補間により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
        moveVelocity_ = 1.0f * (Slerp(moveVelocity_, toPlayer_, 0.1f));

        // 進行方向に見た目の回転を合わせる
        // Y軸周り角度(θy)
        rotation_.y = std::atan2(moveVelocity_.x, moveVelocity_.z);
        rotation_.x = 0.0f;

        moveVelocity_ /= 20.0f;

        moveVelocity_.y = 0.0f;
        position_ += moveVelocity_;

		SetPosition(position_);
		SetRotation(rotation_);
		SetScale(scale_);

		ObjectTransformSet(position_, rotation_, scale_);
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
        bullet->UpdateModel();

        // 弾をリストに追加
        pBullets_.push_back(std::move(bullet));
    }
}
    

void NormalEnemy::ChangeBehaviorState(std::unique_ptr<EnemyBehaviorState> _pState)
{
    pBehaviorState_ = std::move(_pState);
    pBehaviorState_->Initialize();
}

void NormalEnemy::ObjectTransformSet(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale)
{
    object_->SetPosition(_position);
    object_->SetRotate(_rotation);
    object_->SetScale(_scale);
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
        ParticleEmitter::Emit("vignetteGroup", { position_.x, position_.y + 1.0f,position_.z }, 3);

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
