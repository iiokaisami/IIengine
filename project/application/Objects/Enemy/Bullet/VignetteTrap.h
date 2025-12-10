#pragma once

#include "../../../baseObject/GameObject.h"
#include"../../../../gameEngine/collider/ColliderManager.h"

#include <Object3d.h>

/// <summary>
/// ヴィネットトラップ
/// プレイヤーの周囲に放物線を描いて飛んでいきプレイヤーに衝突すると爆発する罠
/// </summary>
class VignetteTrap : public GameObject
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

	// モデル更新
	void UpdateModel();

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
	void OnCollisionTrigger(const Collider* _other);

	/// <summary>
	/// 衝突中処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollision(const Collider* _other);

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

	// 3Dオブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;

	// 当たり判定関係
	ColliderManager* colliderManager_ = nullptr;
	Collider collider_;
	AABB aabb_;
	Collider::ColliderDesc desc = {};
	
	// 壁との衝突フラグ
	bool isWallCollision_ = false;
	// 衝突した壁のAABB
	AABB collisionWallAABB_;
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
	
	
};

