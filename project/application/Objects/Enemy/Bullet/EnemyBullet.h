#pragma once

#include "../../../baseObject/GameObject.h"
#include"../../../../gameEngine/collider/ColliderManager.h"
#include "../../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>

/// <summary>
/// 敵の弾クラス
/// </summary>
class EnemyBullet : public GameObject
{
public:
	EnemyBullet() = default;
	~EnemyBullet() = default;

	// 初期化
	void Initialize() override;
	
	// 終了
	void Finalize() override;
	
	// 更新
	void Update() override;
	
	// 描画
	void Draw() override;
	
	// スプライト描画
	void Draw2D();
	
	// ImGui
	void ImGuiDraw();

	// モデル更新
	void UpdateModel();

private: // 衝突判定
	
	/// <summary>
	/// 衝突時処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollisionTrigger(const Collider* _other);

public: // セッター

	/// <summary>
	/// 速度設定
	/// </summary>
	/// <param name="_velocity">速度ベクトル</param>
	/// <returns>設定後の速度ベクトル</returns>
	Vector3 SetVelocity(const Vector3 _velocity) { return velocity_ = _velocity; }

private: // メンバ変数

	// 3Dオブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;
	
	// 当たり判定関係
	ColliderManager* colliderManager_ = nullptr;
	Collider collider_;
	AABB aabb_;
	Collider::ColliderDesc desc = {};
	
	// 速度
	Vector3 velocity_{};
	
	// 寿命
	static const uint32_t kLifeTime = 60 * 3;

	// デスタイマー
	uint32_t deathTimer_ = kLifeTime;

};

