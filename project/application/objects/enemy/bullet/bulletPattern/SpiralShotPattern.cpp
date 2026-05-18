#include "SpiralShotPattern.h"

void SpiralShotPattern::Fire(const Vector3& _origin, BulletManager& _manager, float _time, const Vector3& _playerPos)
{
    _playerPos;


    // 現在の角度を決める
    float periodSec = 3.0f;
    float angularSpeed = 360.0f / periodSec; // 1秒に何度進むか

    float angle = std::fmod(_time * angularSpeed, 360.0f);

    // 基準を"X+"に
    float rad = angle * 3.14159265f / 180.0f;
    Vector3 dir = { std::cos(rad), 0.0f, std::sin(rad) };

    auto bullet = std::make_unique<EnemyBullet>();
    bullet->SetPosition(_origin);
    bullet->SetVelocity(dir * bulletSpeed_);
    _manager.AddBullet(std::move(bullet));
    
}
