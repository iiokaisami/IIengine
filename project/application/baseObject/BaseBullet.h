#pragma once

#include "GameObject.h"
#include "../../gameEngine/collider/ColliderManager.h"

#include <memory>
#include <functional>
#include <Object3d.h>

/// <summary>
/// 弾の基底クラス
/// Character より軽量に弾専用の共通処理を提供する
/// </summary>
class BaseBullet : public GameObject
{
public:

	BaseBullet() = default;
	virtual ~BaseBullet() = default;

	// 初期化
	virtual void Initialize() override {}
	
	// 終了
	virtual void Finalize() override;
	
	// 更新
	virtual void Update() override;
	
	// 描画
	virtual void Draw() override;

	// 速度設定
	virtual Vector3 SetVelocity(const Vector3 _velocity);

protected:

	// 寿命チェックとタイマー更新（派生 Update 内で呼べる）
	void UpdateLifetime();

	// コライダー削除ユーティリティ
	void DeleteColliderSafe();

protected:

	// デフォルトフレームレート
	static constexpr float kDefaultFrameRate = 60.0f;

	// 当たり判定関係
	ColliderManager* colliderManager_ = nullptr;
	Collider collider_;
	AABB aabb_;
	Collider::ColliderDesc desc = {};

	// 移動速度
	Vector3 velocity_{};

	// 寿命
	uint32_t lifeTimeFrames_ = 60 * 3;

	// デスタイマー
	float deathRemainingSeconds_ = static_cast<float>(lifeTimeFrames_) / kDefaultFrameRate;
};
