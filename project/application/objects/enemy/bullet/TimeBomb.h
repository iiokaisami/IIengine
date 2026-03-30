#pragma once

#include "../../../baseObject/Character.h"
#include"../../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>

/// <summary>
/// 時限爆弾
/// </summary>
class TimeBomb : public Character
{
public:

	TimeBomb() = default;
	~TimeBomb() = default;

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

private:

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnSetCollisionTrigger(const IIEngine::Collider* _other);
	
	/// <summary>
	/// 衝突中処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnSetCollision(const IIEngine::Collider* _other);
	
	/// <summary>
	/// 衝突時処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnExplosionTrigger(const IIEngine::Collider* _other);


	// 爆発処理
	void Explode();

	// 壁との反発処理
	void ReflectOnWallCollision();

public: // ゲッター

public: // セッター

	/// <summary>
	/// 速度設定
	/// </summary>
	/// <param name="_velocity">速度ベクトル</param>
	/// <returns>設定した速度ベクトル</returns>
	void SetVelocity(const Vector3& _velocity)
	{
		velocity_ = _velocity * kDefaultFrameRate;
	}

	/// <summary>
	/// プレイヤーの周囲の座標を設定
	/// </summary>
	/// <param name="_playerPosition">プレイヤーの位置</param>
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
	// 設置判定
	IIEngine::Collider setCollider_;
	IIEngine::AABB setAABB_;
	IIEngine::Collider::ColliderDesc setDesc = {};
	// 爆発判定
	std::string explosionObjectName_;
	IIEngine::Collider explosionCollider_;
	IIEngine::AABB explosionAABB_;
	IIEngine::Collider::ColliderDesc explosionDesc = {};


	// 爆発フラグ
	bool isExploded_ = false;
	// 初期スケールと目標スケール
	Vector3 startScale = { 1.0f, 1.0f, 1.0f };
	Vector3 endScale = { 2.0f, 2.0f, 2.0f };
	// 経過時間とスケール
	float elapsedTime = 0.0f;
	float duration = 1.0f; // 1秒間
	Vector3 currentScale;

	// 相手のHP
	float anyHP_ = 0.0f;

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

	float baseScale_ = 0.7f;

	// 重力加速度
	float gravity_ = -9.8f;
	float landingEpsilon_ = 0.1f;
	float groundY_ = 0.5f;

	// 壁との反発処理
	float wallPushOut_ = 0.5f;
	uint32_t wallCollisionCooldownFrames_ = 1;

	// 爆発のパーティクル関連
	float twoPi_ = std::numbers::pi_v<float> *2.0f;
	float minRadius_ = 2.5f;
	float radiusVariance_ = 1.5f;

	// 爆発のスケールアップ時間
	int explosionParticleCount_ = 6;

	// ゆらゆら用(回転)
	// 揺れの経過時間
	float swayTimer_ = 0.0f;
	// Y回転速度
	float yRotateSpeed_ = 0.10f;
	// 揺れの最大角度
	float swayAmplitude_ = 0.5f;
	// 揺れの速さ
	float swayFrequency_ = 1.5f;

};

