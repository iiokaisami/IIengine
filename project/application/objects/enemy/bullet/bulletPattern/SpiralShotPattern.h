#pragma once

#include <cmath>

#include "BulletPattern.h"
#include "../EnemyBullet.h"

/// <summary>
/// 螺旋弾
/// </summary>
class SpiralShotPattern : public BulletPattern
{
public:

    /// <summary>
	/// コンストラクタ
    /// </summary>
    /// <param name="_spiralSpeed">螺旋の回転速度</param>
    /// <param name="_bulletSpeed">弾の速度</param>
    SpiralShotPattern(float _spiralSpeed, float _bulletSpeed): spiralSpeed_(_spiralSpeed), bulletSpeed_(_bulletSpeed) {}
    
	/// <summary>
	/// 発射
	/// </summary>
	/// <param name="_origin">発射位置</param>
	/// <param name="_manager">弾管理者</param>
	/// <param name="_time">経過時間</param>
	void Fire(const Vector3& _origin, BulletManager& _manager, float _time) override;

private:

	float spiralSpeed_ = 2.0f;
	float bulletSpeed_ = 0.1f;

};

