#include "VignetteTrap.h"

#include <numbers>

void VignetteTrap::Initialize()
{
	// --- 3Dオブジェクト ---
	object_ = std::make_unique<Object3d>();
	object_->Initialize("vignette.obj");

	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	scale_ = { 0.7f,0.7f,0.7f };
	object_->SetScale(scale_);
	// ライト設定
	object_->SetDirectionalLightEnable(true);

	// 当たり判定
	colliderManager_ = ColliderManager::GetInstance();
	objectName_ = "VignetteTrap";

	desc =
	{
		//ここに設定
		.owner = this,
		.colliderID = objectName_,
		.shape = Shape::AABB,
		.shapeData = &aabb_,
		.attribute = colliderManager_->GetNewAttribute(objectName_),
		.onCollision = std::bind(&VignetteTrap::OnCollision, this, std::placeholders::_1),
		.onCollisionTrigger = std::bind(&VignetteTrap::OnCollisionTrigger, this, std::placeholders::_1),
	};
	collider_.MakeAABBDesc(desc);
	colliderManager_->RegisterCollider(&collider_);
}

void VignetteTrap::Finalize()
{
	colliderManager_->DeleteCollider(&collider_);
}

void VignetteTrap::Update()
{
	// 着弾していなければ判定を付けない
	isActive_ = !isLaunchingTrap_;

	// 物理挙動（放物線運動）
	if (isLaunchingTrap_)
	{
		// 重力加速度
		const float gravity = -9.8f;
		// 1フレームの時間
		const float deltaTime = 1.0f / 60.0f;

		// 速度に重力を加算
		velocity_.y += gravity * deltaTime;

		// 位置を速度で更新
		position_ += velocity_ * deltaTime;
	}
	else
	{
		// 地面に埋まらないようにする
		if (position_.y < 0.5f)
		{
			position_.y = 0.5f;
			velocity_.y = 0.0f; // Y方向の速度をリセット
		}
	}

	if ((position_ - landingPosition_).Length() < 0.1f or position_.y <= 0.5f)
	{
		isLaunchingTrap_ = false;
		velocity_ = { 0.0f, 0.0f, 0.0f }; // 着弾時に速度をリセット
	}

	// 壁との反射クールタイム
	if (wallCollisionCooldown_ > 0)
	{
		wallCollisionCooldown_--;
	}

	if (wallCollisionCooldown_ <= 0 && isWallCollision_)
	{
		// 壁に衝突した場合の処理
		ReflectOnWallCollision();
		wallCollisionCooldown_ = 1;
		isWallCollision_ = false;
	}

	UpdateModel();

	rotation_ += {0.1f, 0.1f, 0.0f};

	aabb_.min = position_ - object_->GetScale();
	aabb_.max = position_ + object_->GetScale();
	collider_.SetPosition(position_);
}

void VignetteTrap::Draw()
{
	object_->Draw();
}

void VignetteTrap::ImGuiDraw()
{
}

void VignetteTrap::UpdateModel()
{
	object_->Update();

	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	object_->SetScale(scale_);
}

void VignetteTrap::LaunchTrap()
{
	isLaunchingTrap_ = true;

	// 重力加速度
	const float gravity = -9.8f;

	// XYの差分
	Vector3 diff = landingPosition_ - position_;

	// XZ方向の初速度
	velocity_.x = diff.x / flightTime_;
	velocity_.z = diff.z / flightTime_;

	// Y方向の初速度
	velocity_.y = (diff.y - 0.5f * gravity * flightTime_ * flightTime_) / flightTime_;
}

void VignetteTrap::OnCollisionTrigger(const Collider* _other)
{
	if (isActive_ && (_other->GetColliderID() == "Player" or 
		_other->GetColliderID() == "PlayerBullet" or
		_other->GetColliderID() == "NormalEnemy"))
	{
		// 死亡
		isDead_ = true;
	}
}

void VignetteTrap::OnCollision(const Collider* _other)
{
	if (_other->GetColliderID() == "Wall" or 
		_other->GetColliderID() == "Barrie")
	{
		// 反射処理用のAABBを取得
		collisionWallAABB_ = *_other->GetAABB();
		isWallCollision_ = true;
	}

	if (_other->GetColliderID() == "TrapEnemy" or
		_other->GetColliderID() == "VignetteTrap" or
		_other->GetColliderID() == "SetTimeBomb")
	{
		const AABB* otherAABB = _other->GetAABB();

		if (otherAABB)
		{
			CorrectOverlap(*otherAABB, aabb_, position_);
		}
	}
}

void VignetteTrap::ReflectOnWallCollision()
{
	// 重なり補正と同じく、軸ごとの重なり量を調べる
	float overlapLeftX = collisionWallAABB_.max.x - aabb_.min.x;
	float overlapRightX = aabb_.max.x - collisionWallAABB_.min.x;
	float correctionX = (overlapLeftX < overlapRightX) ? overlapLeftX : -overlapRightX;

	float overlapBackZ = collisionWallAABB_.max.z - aabb_.min.z;
	float overlapFrontZ = aabb_.max.z - collisionWallAABB_.min.z;
	float correctionZ = (overlapBackZ < overlapFrontZ) ? overlapBackZ : -overlapFrontZ;

	float absX = std::abs(correctionX);
	float absZ = std::abs(correctionZ);

	// 目標地点を反転
	if (absX <= absZ)
	{
		landingPosition_.x = position_.x + (position_.x - landingPosition_.x);
		// 壁の外側に十分押し出す
		position_.x += (correctionX + (correctionX > 0 ? 0.5f : -0.5f));
	} 
	else
	{
		landingPosition_.z = position_.z + (position_.z - landingPosition_.z);
		// 壁の外側に十分押し出す
		position_.z += (correctionZ + (correctionZ > 0 ? 0.5f : -0.5f));
	}

	// ここで新しい放物線運動を再計算
	LaunchTrap();
}

void VignetteTrap::SetTrapLandingPosition(const Vector3& _playerPosition)
{
	// ランダムな角度（0〜2π）
	float angle = static_cast<float>(rand()) / RAND_MAX * std::numbers::pi_v<float> *2.0f;

	// ランダムな半径（2.5〜4.0）
	float radius = 2.5f + static_cast<float>(rand()) / RAND_MAX * 1.5f;

	// オフセットを計算（XZ平面）
	Vector3 offset;
	offset.x = std::cos(angle) * radius;
	offset.z = std::sin(angle) * radius;
	offset.y = 0.0f;

	// プレイヤーの位置にオフセットを加え、Y座標を0.5に
	landingPosition_ = _playerPosition + offset;
	landingPosition_.y = 0.5f;
}
