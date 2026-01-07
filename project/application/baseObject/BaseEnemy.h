#pragma once

#include "GameObject.h"

#include <memory>
#include <functional>
#include <Vector3.h>
#include <Sprite.h>

/// <summary>
/// 敵の基底クラス
/// </summary>
class BaseEnemy : public GameObject
{
public:

	BaseEnemy() = default;
	virtual ~BaseEnemy() = default;

	// 初期化
	virtual void Initialize(const std::string& objectName, const Vector3& scale, float initialHP);

	// 更新
	virtual void Update() override;
	
	// 移動
	virtual void Move();

	// HPバー更新
	virtual void UpdateHPBar();

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

	// HPバー
	Vector2 hpBarSize_ = { 100.0f, 20.0f };
	float maxHP_ = 100.0f;
	Vector3 hpBarOffset_ = { 0.0f, 1.6f, 1.5f };
	float hpRatio_ = 1.0f;
	float hpLerpSpeed_ = 10.0f;

	// スプライト
	std::vector<std::unique_ptr<Sprite>> sprites_;
	uint32_t spriteNum_ = 1;

};

