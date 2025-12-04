#include "Collider.h"

#include<sstream>

#include"ColliderManager.h"

Collider::Collider()
{
}

Collider::~Collider()
{
}

void Collider::MakeAABBDesc(const ColliderDesc& desc)
{
    SetOwner(desc.owner);
    SetColliderID(desc.colliderID);
    SetShape(Shape::AABB);
    SetShapeData(static_cast<AABB*>(desc.shapeData));
    SetAttribute(desc.attribute);
    if (desc.onCollision) SetOnCollision(desc.onCollision);
    if (desc.onCollisionTrigger) SetOnCollisionTrigger(desc.onCollisionTrigger);
}

void Collider::MakeOBBDesc(const ColliderDesc& desc)
{
    SetOwner(desc.owner);
    SetColliderID(desc.colliderID);
    SetShape(Shape::OBB);
    SetShapeData(static_cast<OBB*>(desc.shapeData));
    SetAttribute(desc.attribute);
    if (desc.onCollision) SetOnCollision(desc.onCollision);
    if (desc.onCollisionTrigger) SetOnCollisionTrigger(desc.onCollisionTrigger);
}

void Collider::MakeSphereDesc(const ColliderDesc& desc)
{
    SetOwner(desc.owner);
    SetColliderID(desc.colliderID);
    SetShape(Shape::Sphere);
    SetShapeData(static_cast<Sphere*>(desc.shapeData));
    SetAttribute(desc.attribute);
    if (desc.onCollision) SetOnCollision(desc.onCollision);
    if (desc.onCollisionTrigger) SetOnCollisionTrigger(desc.onCollisionTrigger);
}

const bool Collider::IsRegisteredCollidingPtr(const Collider* _ptr) const
{
    for (auto itr = collidingPtrList_.begin(); itr != collidingPtrList_.end(); ++itr)
    {
        if (_ptr == *itr) return true;
    }
    return false;
}

void Collider::EraseCollidingPtr(const Collider* _ptr)
{
    collidingPtrList_.remove_if([_ptr](const Collider* _pCollider) 
        {
        return _pCollider == _ptr;
        });
    return;
}

void Collider::SetAttribute(uint32_t _attribute)
{
    collisionAttribute_ = _attribute;
}

void Collider::SetMask(uint32_t _mask)
{
    collisionMask_ = _mask;
}

void Collider::OnCollisionTrigger(const Collider* _other)
{
    if (onCollisionTriggerFunction_)
        onCollisionTriggerFunction_(_other);
    return;
}
