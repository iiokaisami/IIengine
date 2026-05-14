#include "LaserPattern.h"

void LaserPattern::Fire(const Vector3& _origin, BulletManager& _manager, float _time)
{
    _time;

    auto laser = std::make_unique<EnemyBullet>();
    laser->SetPosition(_origin);
    laser->SetVelocity(direction_ * speed_);
    _manager.AddBullet(std::move(laser));
}
