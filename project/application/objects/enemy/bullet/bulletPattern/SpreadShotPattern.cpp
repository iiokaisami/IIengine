#include "SpreadShotPattern.h"

void SpreadShotPattern::Fire(const Vector3& _origin, BulletManager& _manager, float _time, const Vector3& _playerPos)
{
    _time;

    // プレイヤー方向ベクトル
    Vector3 toPlayer = _playerPos - _origin;
    toPlayer.y = 0; // Y成分無視
    Vector3 dir = Normalize(toPlayer);

    // 角度(目標方向のY回転)を求める
    float baseAngle = atan2(dir.z, dir.x) * 180.0f / 3.14159265f;

	// 発射角度の計算
    float startAngle = baseAngle - spreadAngle_ * 0.5f;
    float angleStep = bulletCount_ > 1 ? (spreadAngle_ / (bulletCount_ - 1)) : 0.0f;

	// 弾の生成と発射
    for (uint32_t i = 0; i < bulletCount_; ++i) 
    {
        float angle = startAngle + angleStep * i;
        float rad = angle * 3.14159265f / 180.0f;
        Vector3 shotDir = { std::cos(rad), 0.0f, std::sin(rad) };

        auto bullet = std::make_unique<EnemyBullet>();
        bullet->SetPosition(_origin);
        bullet->SetVelocity(shotDir * speed_);
        _manager.AddBullet(std::move(bullet));
    }
}
