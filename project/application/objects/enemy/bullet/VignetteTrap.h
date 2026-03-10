#pragma once

#include <numbers>

#include "../../../baseObject/Character.h"
#include"../../../../gameEngine/collider/ColliderManager.h"

#include <Object3d.h>

/// <summary>
/// ヴィネットトラップ
/// プレイヤーの周囲に放物線を描いて飛んでいきプレイヤーに衝突すると爆発する罠
/// </summary>
class VignetteTrap : public Character
{
public:

	VignetteTrap() = default;
	~VignetteTrap() = default;

	// 初期化
	void Initialize() override;
	
	// 終了
	void Finalize() override;
	
	// 更新
	void Update() override;
	
	// 描画
	void Draw() override;
	
	// ImGui
	void ImGuiDraw();

	// 放物線上に発射
	void LaunchTrap();

public: // 内部関数

	// 消滅時のリアクション
	void DeadMotion();

private: // 衝突判定

	/// <summary>
	/// 衝突時処理
	/// </summary>
	/// <param name="_other>衝突相手のコライダー</param>
	void OnCollisionTrigger(const IIEngine::Collider* _other);

	/// <summary>
	/// 衝突中処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollision(const IIEngine::Collider* _other);

	// 壁との反発処理
	void ReflectOnWallCollision();

public: // ゲッター


public: // セッター

	/// <summary>
	/// 速度設定
	/// </summary>
	/// <param name="_velocity">速度ベクトル</param>
	/// <returns></returns>
	Vector3 SetVelocity(const Vector3 _velocity) { return velocity_ = _velocity; }

	// プレイヤーの周囲の座標を設定
	void SetTrapLandingPosition(const Vector3& _playerPosition);

private:

	// 消滅時の動作構造体
	struct DeathMotion
	{
		bool isActive = false;
		float timer = 0.0f;
		Vector3 motionPos = {};
	};

	// 消滅モーション調整パラメーター
	static constexpr float kVibrationFreq = 200.0f;    // 振動周波数
	static constexpr float kVibrationAmp = 0.2f;     // 振動幅(±px)

	static constexpr float kShrinkFactor = 0.92f;    // 縮小率(指数減衰)
	static constexpr float kEndScale = 0.05f;    // 消滅判定スケール
	// 消滅時の動作
	DeathMotion deathMotion_{};

	// デフォルトのフレームレート
	const float kDefaultFrameRate = 60.0f;

	// 当たり判定関係
	IIEngine::ColliderManager* colliderManager_ = nullptr;
	IIEngine::Collider collider_;
	IIEngine::AABB aabb_;
	IIEngine::Collider::ColliderDesc desc = {};
	
	// 壁との衝突フラグ
	bool isWallCollision_ = false;
	// 衝突した壁のAABB
	IIEngine::AABB collisionWallAABB_;
	// 反射のクールタイム
	uint32_t wallCollisionCooldown_ = 0;

	// 着弾地点
	Vector3 landingPosition_{};
	// 速度
	Vector3 velocity_{};
	// 発射中フラグ
	bool isLaunchingTrap_ = false;
	// 着弾までの時間
	float flightTime_ = 1.0f;

	//寿命
	static const int32_t kLifeTime = 60 * 12;
	// デスタイマー
	float deathRemainingSeconds_ = kLifeTime / kDefaultFrameRate;

	// 基本スケール
	float baseScale_ = 0.7f;

	// 物理挙動調整パラメーター
	float gravity_ = -9.8f;
	float landingEpsilon_ = 0.1f;
	float groundY_ = 0.5f;

	// 壁からの押し出し量
	float wallPushOut_ = 0.5f;
	uint32_t wallCollisionCooldownFrames_ = 1;

	// 放物線のパラメーター
	float TwoPi_ = std::numbers::pi_v<float> *2.0f;
	float minRadius_ = 2.5f;
	float radiusVariance_ = 1.5f;

};

