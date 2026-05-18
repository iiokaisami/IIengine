#pragma once

#include <cmath>

#include "BulletPattern.h"
#include "../EnemyBullet.h"

/// <summary>
/// 拡散弾パターン
/// </summary>
class SpreadShotPattern : public BulletPattern
{
public:

    /// <summary>
	/// コンストラクタ
    /// </summary>
    /// <param name="_bulletCount">弾の数</param>
    /// <param name="_spreadAngle">拡散角度</param>
    /// <param name="_speed">弾の速度</param>
    SpreadShotPattern(uint32_t _bulletCount, float _spreadAngle, float _speed): bulletCount_(_bulletCount), spreadAngle_(_spreadAngle), speed_(_speed) {}

    /// <summary>
	/// 弾の発射
    /// </summary>
    /// <param name="_origin">発射位置</param>
    /// <param name="_manager">弾の管理者</param>
    /// <param name="_time">発射時間</param>
    void Fire(const Vector3& _origin, BulletManager& _manager, float _time, const Vector3& _playerPos) override;
    
private:
    
	// 弾の数
    uint32_t bulletCount_;
	// 拡散角度
    float spreadAngle_;
	// 弾の速度
    float speed_;

};
