#pragma once

#include "../../baseObject/GameObject.h"
#include "bullet/EnemyBullet.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorState.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Sprite.h>
#include <Framework.h>

/// <summary>
/// 通常敵
/// 行動ステートを持つ
/// </summary>
class NormalEnemy : public GameObject
{
public:

	NormalEnemy() = default;
	~NormalEnemy() = default;

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

	// 移動
	void Move();

	// 攻撃
	void Attack();

	/// <summary>
	/// 行動ステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステートポインタ</param>
	void ChangeBehaviorState(std::unique_ptr<EnemyBehaviorState> _pState);

private: // 衝突判定

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollisionTrigger(const Collider* _other);

	/// <summary>
	/// 衝突中の処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollision(const Collider* _other);

	// 暗闇トラップに衝突したときの処理
	void HitVignetteTrap();

public: // ゲッター

	// プレイヤーとの距離
	Vector3 GetToPlayer() const { return toPlayer_; }

	// 被弾フラグ
	bool IsHit() const { return isHit_; }

	// プレイヤーとの距離が一定以上かどうか
	bool IsFarFromPlayer() const { return isFarFromPlayer_; }

	// 暗闇フラグ
	bool IsHitVignetteTrap() const { return isHitVignetteTrap_; }

public: // セッター

	// プレイヤーの位置をセット
	void SetPlayerPosition(Vector3 _playerPosition) { playerPosition_ = _playerPosition; }

	// 無敵フラグをセット
	void SetIsInvincible(bool _isInvincible) { isInvincible_ = _isInvincible; }

	// 被弾フラグをセット
	void SetIsHit(bool _isHit) { isHit_ = _isHit; }

	/// <summary>
	/// objectのtransformをセット
	/// </summary>
	/// <param name="_position">位置</param>
	/// <param name="_rotation">回転</param>
	/// <param name="_scale">スケール</param>
	void ObjectTransformSet(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale);

	/// 以下1つづつセット ///
	
	// オブジェクトのpositionをセット
	void SetObjectPosition(const Vector3& _position) { object_->SetPosition(_position); }
	// オブジェクトのrotationをセット
	void SetObjectRotation(const Vector3& _rotation) { object_->SetRotate(_rotation); }
	// オブジェクトのscaleをセット
	void SetObjectScale(const Vector3& _scale) { object_->SetScale(_scale); }


private:

	static constexpr float kDefaultFrameRate = 60.0f;

	// 3Dオブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;

	// 当たり判定関係
	ColliderManager* colliderManager_ = nullptr;
	Collider collider_;
	AABB aabb_;
	Collider::ColliderDesc desc = {};

	// 移動速度
	Vector3 moveVelocity_{};
	float moveSpeed_ = 0.05f;

	// プレイヤーの位置
	Vector3 playerPosition_{};
	Vector3 toPlayer_{};
	// 追尾停止距離
	const float kStopChasingDistance = 15.0f;

	// 弾
	std::vector<std::unique_ptr<EnemyBullet>> pBullets_ = {};

	// 行動ステート
	std::unique_ptr<EnemyBehaviorState> pBehaviorState_ = nullptr;

	// 無敵フラグ(出現時等攻撃を受けなくさせる)
	bool isInvincible_ = true;

	// 被弾フラグ
	bool isHit_ = false;

	// プレイヤーとの距離が一定以上かどうか
	bool isFarFromPlayer_ = false;

	// 暗闇トラップに当たったかどうか
	bool isHitVignetteTrap_ = false;
	// 暗闇効果最大時間
	const uint32_t kMaxVignetteTime = 60 * 3;
	// 暗闇タイマー
	uint32_t vignetteTime_ = kMaxVignetteTime;


};

