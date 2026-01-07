#include "StaticObject.h"

void StaticObject::Initialize(const std::string& _modelFileName, const std::string& _objectName)
{
    // Object3d の初期化
    object_ = std::make_unique<Object3d>();
    object_->Initialize(_modelFileName);

    // 初期位置、回転、スケールを設定
    object_->SetPosition(position_);
    object_->SetRotate(rotation_);
    object_->SetScale(scale_);

    // 当たり判定の初期化
    colliderManager_ = ColliderManager::GetInstance();

    objectName_ = _objectName;

    // デフォルトのCollider設定
    Collider::ColliderDesc desc = {
        .owner = this,
        .colliderID = objectName_,
        .shape = Shape::AABB,
        .shapeData = &aabb_,
        .attribute = colliderManager_->GetNewAttribute(objectName_),
        .onCollisionTrigger = std::bind(&StaticObject::OnCollisionTrigger, this, std::placeholders::_1),

    };
    collider_.MakeAABBDesc(desc);
    colliderManager_->RegisterCollider(&collider_);
}

void StaticObject::Finalize()
{
    colliderManager_->DeleteCollider(&collider_);
}

void StaticObject::Update()
{
    // 固定オブジェクトとして標準では更新処理なし
	object_->SetPosition(position_);
	object_->SetRotate(rotation_);
	object_->SetScale(scale_);
    object_->Update();

    aabb_.min = position_ - object_->GetScale();
    aabb_.max = position_ + object_->GetScale();
	aabb_.max.y += 1.0f; // 見た目よりも縦に大きく
    collider_.SetPosition(position_);
}

void StaticObject::Draw() 
{
    object_->Draw();
}