#pragma once

#include <Vector3.h>

#include "BulletPattern.h"
#include "../EnemyBullet.h"

/// <summary>
/// レーザーパターン
/// </summary>
class LaserPattern : public BulletPattern
{
public:

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="_direction">レーザーの方向</param>
    /// <param name="_speed">レーザーの速度</param>
    LaserPattern(const Vector3& _direction, float _speed): direction_(_direction.Normalize()), speed_(_speed) {}

    /// <summary>
	/// 発射
    /// </summary>
	/// <param name="_origin">発射位置</param>
	/// <param name="_manager">弾の管理クラス</param>
	/// <param name="_time">発射時間</param>
    void Fire(const Vector3& _origin, BulletManager& _manager, float _time) override;
    
private:
    
    Vector3 direction_;
    float speed_;

};

