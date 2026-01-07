#pragma once

#include "Character.h"

#include <memory>
#include <functional>
#include <Vector3.h>
#include <Framework.h>

/// <summary>
/// 敵の基底クラス
/// </summary>
class BaseEnemy : public Character
{
public:

	BaseEnemy() = default;
	virtual ~BaseEnemy() = default;

	// 初期化
	virtual void Initialize(const std::string& _modelFileName, const std::string& _objectName, float _initialHP);

	// 更新
	virtual void Update() override;
	
	// 移動
	virtual void Move() override;

protected:

	// 進行方向の補間処理
	Vector3 InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate);

protected:

    // 移動関連
    Vector3 moveVelocity_;
    float moveSpeed_;

    // プレイヤー関連
    Vector3 playerPosition_;
    Vector3 toPlayer_;

    // フレームレート補助
    static constexpr float kDefaultFrameRate = 60.0f;

};

