#include "SpiralShotPattern.h"

void SpiralShotPattern::Fire(const Vector3& _origin, BulletManager& _manager, float _time)
{
    float angle = _time * spiralSpeed_;
    float rad = angle * 3.14159265f / 180.0f;
    Vector3 dir = { std::cos(rad), 0.0f, std::sin(rad) };

    auto bullet = std::make_unique<EnemyBullet>();
    bullet->SetPosition(_origin);
    bullet->SetVelocity(dir * bulletSpeed_);
    _manager.AddBullet(std::move(bullet));
    
}
