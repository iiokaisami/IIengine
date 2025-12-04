#pragma once

#include "../../../baseObject/GameObject.h"
#include "../../../../gameEngine/collider/ColliderManager.h"
#include "../../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Sprite.h>

/// <summary>
/// プレイヤー弾クラス
/// </summary>
class PlayerBullet : public GameObject
{
public:

	PlayerBullet() = default;
	~PlayerBullet() = default;
	
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
	
private: // 衝突判定

	/// <summary>
	/// 衝突時処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollisionTrigger(const Collider* _other);

public: // ゲッター

	/// <summary>
	/// 速度取得
	/// </summary>
	/// <returns>速度ベクトル</returns>
	Vector3 SetVelocity(const Vector3 _velocity)
	{
		velocity_ = _velocity * kDefaultFrameRate; 
		return velocity_; 
	}

public: // セッター


private: // メンバ変数

	static constexpr float kDefaultFrameRate = 60.0f;

	// 3Dオブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;

	// 当たり判定関係
	ColliderManager* colliderManager_ = nullptr;
	Collider collider_;
	AABB aabb_;
	Collider::ColliderDesc desc = {};

	// 速度
	Vector3 velocity_{};

	//寿命<frm>
	static const int32_t kLifeTime = 60 * 5;

	//デスタイマー
	//int32_t deathTimer_ = kLifeTime;
	float deathRemainingSeconds_ = kLifeTime / kDefaultFrameRate;
};