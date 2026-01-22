#include "ColliderManager.h"

#include <Vector3.h>
#include <Matrix4x4.h>
#include <cmath>
#include <algorithm>

#include"MyMath.h"

void ColliderManager::Initialize()
{
}

void ColliderManager::CheckAllCollision()
{
    collisionNames_.clear();
    countCheckCollision_ = 0ui32;
    countWithoutFilter_ = 0ui32;
    countWithoutLighter = 0ui32;

    auto itrA = colliders_.begin();
    for (; itrA != colliders_.end(); ++itrA)
    {
        auto itrB = itrA + 1;
        for (; itrB != colliders_.end(); ++itrB)
        {
            CheckCollisionPair(*itrA, *itrB);
        }
    }
}

void ColliderManager::RegisterCollider(Collider* _collider)
{
    colliders_.push_back(_collider);
}

void ColliderManager::ClearColliderList()
{
    colliders_.clear();
}

void ColliderManager::DeleteCollider(Collider* _collider)
{
    for (int i = 0; i < colliders_.size(); i++)
    {
        colliders_[i]->EraseCollidingPtr(_collider);
        if (colliders_[i] == _collider)
        {
            colliders_.erase(colliders_.begin() + i);
        }
    }
}

uint32_t ColliderManager::GetNewAttribute(std::string _id)
{
    if (attributeList_.size() == 0)
    {
        attributeList_.push_back({ _id, 1 });
        return 1;
    }
    for (auto& attributePair : attributeList_)
    {
        if (attributePair.first.compare(_id) == 0)
        {
            return attributePair.second;
        }
    }

    uint32_t result = static_cast<uint32_t>(attributeList_.back().second << 1);

    attributeList_.push_back({ _id, result });

    return attributeList_.back().second;
}

void ColliderManager::CheckCollisionPair(Collider* _colA, Collider* _colB)
{
    // 衝突しているかどうか
    bool isCollide = true;

    if (!_colA->GetEnable() || !_colB->GetEnable())
    {
        _colA->EraseCollidingPtr(_colB);
        _colB->EraseCollidingPtr(_colA);
        return;
    }
    countWithoutFilter_++;

    // 衝突フィルタリング
    bool fillterFlag =
        !(_colA->GetCollisionAttribute() & _colB->GetCollisionMask()) ||
        !(_colB->GetCollisionAttribute() & _colA->GetCollisionMask());
    if (fillterFlag) return;

    // 形状条件
    if (_colA->GetShape() == Shape::AABB && _colB->GetShape() == Shape::AABB)
    {
        isCollide = IsCollision(_colA->GetAABB(), _colB->GetAABB());
    } else if (_colA->GetShape() == Shape::Sphere && _colB->GetShape() == Shape::Sphere)
    {
        isCollide = IsCollision(_colA->GetSphere(), _colB->GetSphere());
    } else if (_colA->GetShape() == Shape::OBB && _colB->GetShape() == Shape::OBB)
    {
        ++countWithoutLighter;
        /// ラグ軽減のため、半径で判定とって早期リターン (ただし設定されていたら)
        if (_colA->GetRadius() && _colB->GetRadius() && _colA->GetIsEnableLighter() && _colB->GetIsEnableLighter())
        {
            Sphere sphereA = { _colA->GetPosition(), static_cast<float>(_colA->GetRadius()) };
            Sphere sphereB = { _colB->GetPosition(), static_cast<float>(_colB->GetRadius()) };
            isCollide = IsCollision(&sphereA, &sphereB);
        }
        ++countCheckCollision_;


        isCollide = IsCollision(_colA->GetOBB(), _colB->GetOBB());
    }

    if (isCollide)
    {
        _colA->OnCollision(_colB);
        _colB->OnCollision(_colA);

        if (!_colA->IsRegisteredCollidingPtr(_colB) && !_colB->IsRegisteredCollidingPtr(_colA))
        {
            _colA->RegisterCollidingPtr(_colB);
            _colB->RegisterCollidingPtr(_colA);
            _colA->OnCollisionTrigger(_colB);
            _colB->OnCollisionTrigger(_colA);
        }

        collisionNames_.push_back({ _colA->GetColliderID(), _colB->GetColliderID() });
    } else
    {
        // あたっていない場合、CollidingPtrをチェックし該当する場合ポップ
        _colA->EraseCollidingPtr(_colB);
        _colB->EraseCollidingPtr(_colA);
    }
    return;
}

void ColliderManager::ProjectShapeOnAxis(const std::vector<Vector3>* _v, const Vector3& _axis, float& _min, float& _max)
{
    _min = (*_v)[0].Projection(_axis);
    _max = _min;

    for (int i = 1; i < _v->size(); i++)
    {
        float projection = (*_v)[i].Projection(_axis);
        if (projection < _min) _min = projection;
        if (projection > _max) _max = projection;
    }
    return;
}

bool ColliderManager::IsCollision(const AABB* _aabb1, const AABB* _aabb2)
{
    if (_aabb1->max.x >= _aabb2->min.x && _aabb1->min.x <= _aabb2->max.x &&
        _aabb1->max.y >= _aabb2->min.y && _aabb1->min.y <= _aabb2->max.y &&
        _aabb1->max.z >= _aabb2->min.z && _aabb1->min.z <= _aabb2->max.z)
    {
        return true;
    }

    return false;
}

bool ColliderManager::IsCollision(const AABB& _aabb, const Sphere& _sphere)
{
    Vector3 _closestPoint{
       std::clamp(_sphere.center.x, _aabb.min.x, _aabb.max.x),
       std::clamp(_sphere.center.y, _aabb.min.y, _aabb.max.y),
       std::clamp(_sphere.center.z, _aabb.min.z, _aabb.max.z)
    };
    float distance = (_closestPoint - _sphere.center).LengthWithoutRoot();
    if (distance <= _sphere.radius * _sphere.radius)
    {
        return true;
    }
    return false;
}

bool ColliderManager::IsCollision(const Sphere* _sphere1, const Sphere* _sphere2)
{
    Vector3 distanceAB = _sphere1->center - _sphere2->center;
    float radiusAB = _sphere1->radius + _sphere2->radius;
    if (distanceAB.LengthWithoutRoot() < static_cast<float>(radiusAB * radiusAB)) return true;
    return false;
}

bool ColliderManager::IsCollision(const OBB* _obb1, const OBB* _obb2)
{
    // 分離軸のリスト
    Vector3 axes[15];
    int axisIndex = 0;

    // 各OBBのローカル軸（6軸）
    for (int i = 0; i < 3; ++i) {
        axes[axisIndex++] = _obb1->orientations[i];
        axes[axisIndex++] = _obb2->orientations[i];
    }

    // 外積から生成される軸（9軸）
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            axes[axisIndex++] = _obb1->orientations[i].Cross(_obb2->orientations[j]);
        }
    }

    // 各軸について投影を確認
    for (int i = 0; i < 15; ++i) {
        if (!OverlapOnAxis(_obb1, _obb2, axes[i])) {
            // 分離軸が見つかった
            return false;
        }
    }

    // 分離軸がない場合、交差している
    return true;
}

bool ColliderManager::IsCollision(const OBB& _obb, const Sphere& _sphere)
{
    Matrix4x4 obbWorldMatrix{};
        for (int i = 0; i < 3; i++)
            obbWorldMatrix.m[0][i] = _obb.orientations[i].x;
        for (int i = 0; i < 3; i++)
            obbWorldMatrix.m[1][i] = _obb.orientations[i].y;
        for (int i = 0; i < 3; i++)
            obbWorldMatrix.m[2][i] = _obb.orientations[i].z;
        for (int i = 0; i < 3; i++)
            obbWorldMatrix.m[3][i] = *(&_obb.center.x + i);
        obbWorldMatrix.m[3][3] = 1.0f;
    
        Matrix4x4 obbWorldMatrixInverse = obbWorldMatrix.Inverse();
        Vector3 centerInOBBLocalSpace = Transform(_sphere.center, obbWorldMatrixInverse);
    
        AABB aabbOBBLocal{ .min = -_obb.size, .max = _obb.size };
        Sphere sphereOBBLocal{ centerInOBBLocalSpace, _sphere.radius };
    
        return IsCollision(aabbOBBLocal, sphereOBBLocal);
}

float ColliderManager::ProjectOntoAxis(const OBB* _obb, const Vector3& axis)
{
    // 軸方向にOBBの各半サイズを投影
    float extent = 0.0f;

    extent += std::abs(_obb->orientations[0].Dot(axis)) * _obb->size.x;
    extent += std::abs(_obb->orientations[1].Dot(axis)) * _obb->size.y;
    extent += std::abs(_obb->orientations[2].Dot(axis)) * _obb->size.z;

    return extent;
}

bool ColliderManager::OverlapOnAxis(const OBB* _obb1, const OBB* _obb2, const Vector3& axis)
{
    // 軸がゼロベクトルなら無効
    if (axis.x == 0 && axis.y == 0 && axis.z == 0) return true;

    // 軸を正規化
    Vector3 normAxis = axis.Normalize();

    // OBBの中心間距離を軸上に投影
    float distance = std::abs((_obb2->center - _obb1->center).Dot(normAxis));

    // 各OBBの投影幅を計算
    float aProjection = ProjectOntoAxis(_obb1, normAxis);
    float bProjection = ProjectOntoAxis(_obb2, normAxis);

    // 投影区間が重なるか判定
    return distance <= (aProjection + bProjection);
}
