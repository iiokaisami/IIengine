#include "Corruptor.h"

// BehaviorState
#include "behaviorState/corruptorState/CorruptorBehaviorSpawn.h"
#include "behaviorState/corruptorState/CorruptorBehaviorMove.h"
#include "behaviorState/corruptorState/CorruptorBehaviorSelfDestruct.h"

#include "TimeManager.h"

using namespace IIEngine;

void Corruptor::Initialize()
{
	// Initialize を呼び出し共通プロパティを初期化
	BaseEnemy::Initialize("bomb.obj","Corruptor", initialHP_);

	scale_ = initialScale_;
	SyncObjectTransform();
	// ライト設定
	object_->SetLighting(true);

	// 行動ステート
	pBehaviorState_ = std::make_unique<CorruptorBehaviorSpawn>(this);

	// 出現時は無敵状態
	isInvincible_ = true;

	// パーティクル
	IIEngine::ParticleEmitter::Emit("laserGroup", position_, spawnParticleCount_);
}

void Corruptor::Finalize()
{
	colliderManager_->DeleteCollider(&collider_);
}

void Corruptor::Update()
{
	// 基底クラスの共通更新処理を呼び出し
	BaseEnemy::Update();

	// デルタタイム取得
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// アクティブフラグ
	isActive_ = !isInvincible_;

	// 行動ステート更新
	pBehaviorState_->Update();
	
	// モデル変形の更新
	object_->Update();


	// プレイヤーとの距離更新
	float distanceToPlayer = position_.Distance(playerPosition_);

	// プレイヤーとの距離が一定以上かどうかのフラグ更新
	isFarFromPlayer_ = (distanceToPlayer < nearPlayerDistance_);

	if (isExploded_)
	{
		// 爆発後に少しだけスケールを大きくする
		scale_ += Vector3(explodedScaleGrowPerFrame_, explodedScaleGrowPerFrame_, explodedScaleGrowPerFrame_) * dt * kDefaultFrameRate;
	}

}

void Corruptor::Draw()
{
	if (!isExploded_)
	{
		Character::Draw();
	}
}

void Corruptor::ImGuiDraw()
{
#ifdef USE_IMGUI

	ImGui::Begin("Corruptor");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", position_.x, position_.y, position_.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", rotation_.x, rotation_.y, rotation_.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", scale_.x, scale_.y, scale_.z);
	ImGui::Text("HP: %d", hp_);
	ImGui::Text("Is Dead: %s", isDead_ ? "Yes" : "No");
	ImGui::End();

#endif // USE_IMGUI
}

void Corruptor::Move()
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

void Corruptor::ChangeBehaviorState(std::unique_ptr<CorruptorBehaviorState>&& _pState)
{
	pBehaviorState_ = std::move(_pState);
	pBehaviorState_->Initialize();
}

void Corruptor::OnCollisionTrigger(const Collider* _other)
{
	if (_other->GetColliderID() == "PlayerBullet" && !isInvincible_)
	{
		// 弾の衝突時にダメージを受ける処理
		ReceiveBulletDamage(_other);
	}

	if (_other->GetColliderID() == "NormalEnemy" or
		_other->GetColliderID() == "TrapEnemy" or
		_other->GetColliderID() == "Corruptor")
	{

		// 敵同士の衝突時に距離を保つように押し出す処理
		SeparateFromEnemy(_other, separationDistance_);
	}
}

void Corruptor::OnCollision(const Collider* _other)
{
	if (_other->GetColliderID() == "Wall" or
		_other->GetColliderID() == "Barrier")
	{
		// 衝突時に押し出す処理
		ResolveWallCollision(_other);
	}
}