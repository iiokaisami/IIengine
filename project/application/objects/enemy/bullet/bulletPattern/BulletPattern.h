#pragma once

#include <Vector3.h>

#include "BulletManager.h"

/// <summary>
/// 弾のパターンを管理するクラス
/// </summary>
class BulletPattern
{
public:

	virtual ~BulletPattern() = default;

	/// <summary>
	/// 発射
	/// </summary>
	/// <param name="_origin">発射位置</param>
	/// <param name="_manager">弾の管理クラス</param>
	/// <param name="_time">発射時間</param>
	virtual void Fire(const Vector3& _origin, BulletManager& _manager, float _time) = 0;

};

