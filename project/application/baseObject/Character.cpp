#include "Character.h"

#include "TimeManager.h"

void Character::Initialize(const std::string& _modelFileName, const std::string& _objectName, float _initialHP)
{
    // Object3d の初期化
    object_ = std::make_unique<Object3d>();
    object_->Initialize(_modelFileName);

    // 初期位置、回転、スケールを設定
	SyncObjectTransform();

    // 当たり判定の初期化
    colliderManager_ = ColliderManager::GetInstance();

	objectName_ = _objectName;

    Collider::ColliderDesc desc = {
      .owner = this,
      .colliderID = objectName_,
      .shape = Shape::AABB,
      .shapeData = &aabb_,
      .attribute = colliderManager_->GetNewAttribute(objectName_),
      .onCollision = std::bind(&Character::OnCollision, this, std::placeholders::_1),
      .onCollisionTrigger = std::bind(&Character::OnCollisionTrigger, this, std::placeholders::_1), 
    };

    collider_.MakeAABBDesc(desc);
    colliderManager_->RegisterCollider(&collider_);

    // 設定
    maxHP_ = _initialHP;
    hp_ = _initialHP;
    isDead_ = false;
}

void Character::Finalize()
{
    colliderManager_->DeleteCollider(&collider_);
}

void Character::Update()
{
	// Object3d更新
    SyncObjectTransform();

	// collider更新
    aabb_.min = position_ - scale_;
    aabb_.max = position_ + scale_;
    aabb_.max.y += 1.0f;
    collider_.SetPosition(position_);
}

void Character::Draw()
{
    object_->Draw();
}

void Character::Draw2D()
{
    for (auto& sprite : sprites_)
    {
        sprite->Draw();
    }
}

void Character::UpdateHPBar()
{
    auto camera = CameraManager::GetInstance().GetActiveCamera();

    if (!camera or sprites_.empty())
    {
        return;
    }

    // 目標HPを計算
    float targetRatio = 1.0f;
    if (maxHP_ > 0.0f)
    {
        targetRatio = std::clamp(hp_ / maxHP_, 0.0f, 1.0f);
    }

    // 補間
    const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();
    // 補間係数
    float t = std::clamp(hpLerpSpeed_ * dt, 0.0f, 1.0f);
    hpRatio_ += (targetRatio - hpRatio_) * t;

    // スクリーン位置計算
    Vector3 worldCenter = position_ + hpBarOffset_;
    Vector2 screenCenter = camera->WorldToScreen(worldCenter);

    // 左寄せに合わせて左端位置を決める
    float baseWidth = hpBarSize_.x;
    Vector2 leftPos = screenCenter;
    leftPos.x = screenCenter.x - (baseWidth * 0.5f);

    // 幅を決める
    Vector2 newSize = hpBarSize_;
    newSize.x = baseWidth * hpRatio_;

    // 反映
    sprites_[0]->SetPosition(leftPos);
    sprites_[0]->SetSize(newSize);
    sprites_[0]->Update();
}

void Character::CorrectOverlap(const AABB& otherAABB, AABB& selfAABB, Vector3& position)
{
    Vector3 penetrationVector{};

    // x軸(左右)
    float overlapLeftX = otherAABB.max.x - selfAABB.min.x;
    float overlapRightX = selfAABB.max.x - otherAABB.min.x;
    float correctionX = (overlapLeftX < overlapRightX) ? overlapLeftX : -overlapRightX;

    // y軸(上下)
    float overlapBelowY = otherAABB.max.y - selfAABB.min.y;
    float overlapAboveY = selfAABB.max.y - otherAABB.min.y;
    float correctionY = (overlapBelowY < overlapAboveY) ? overlapBelowY : -overlapAboveY;

    // z軸(前後)
    float overlapBackZ = otherAABB.max.z - selfAABB.min.z;
    float overlapFrontZ = selfAABB.max.z - otherAABB.min.z;
    float correctionZ = (overlapBackZ < overlapFrontZ) ? overlapBackZ : -overlapFrontZ;

    // 最小の重なりを持つ軸を選択
    float absX = std::abs(correctionX);
    float absY = std::abs(correctionY);
    float absZ = std::abs(correctionZ);

    if (absX <= absY && absX <= absZ)
        penetrationVector.x = correctionX;
    else if (absY <= absZ)
        penetrationVector.y = correctionY;
    else
        penetrationVector.z = correctionZ;

    // 位置を修正
    position += penetrationVector;
}

bool Character::IsAABBOverlap(const AABB& a, const AABB& b)
{
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}