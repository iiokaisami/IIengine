#pragma once

#include "../../baseObject/BaseEnemy.h"
#include "bullet/EnemyBullet.h"
#include "behaviorState/normalEnemyState/EnemyBehaviorState.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Framework.h>

/// <summary>
/// 通常敵
/// 行動ステートを持つ
/// </summary>
class NormalEnemy : public BaseEnemy
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

	// ImGui
	void ImGuiDraw();

	// 移動
	void Move() override;

	// 攻撃
	void Attack() override;

	/// <summary>
	/// 行動ステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステートポインタ</param>
	void ChangeBehaviorState(std::unique_ptr<EnemyBehaviorState>&& _pState);

private: // 衝突判定

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollisionTrigger(const IIEngine::Collider* _other)override;

	/// <summary>
	/// 衝突中の処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollision(const IIEngine::Collider* _other)override;

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
	void SetPlayerPosition(const Vector3& _playerPosition) { playerPosition_ = _playerPosition; }

	// 無敵フラグをセット
	void SetIsInvincible(bool _isInvincible) { isInvincible_ = _isInvincible; }

	// 被弾フラグをセット
	void SetIsHit(bool _isHit) { isHit_ = _isHit; }


	// --- ステートでのTransform操作用関数(引数あり) ---

	// オブジェクトの positionをセット
	void SetObjectPosition(const Vector3& _position) { object_->SetPosition(_position); }
	// オブジェクトの rotationをセット
	void SetObjectRotation(const Vector3& _rotation) { object_->SetRotate(_rotation); }
	// オブジェクトの scaleをセット
	void SetObjectScale(const Vector3& _scale) { object_->SetScale(_scale); }


private:

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

	// CDタイマー
	float cooldownTimer_ = 0.0f;

	// プレイヤーとの距離が一定以上かどうか
	bool isFarFromPlayer_ = false;

	// 暗闇トラップに当たったかどうか
	bool isHitVignetteTrap_ = false;
	// 暗闇効果最大時間
	const uint32_t kMaxVignetteTime = 60 * 3;
	// 暗闇タイマー
	uint32_t vignetteTime_ = kMaxVignetteTime;

	// 前フレームの位置
	Vector3 lastPosition_;
	int stuckFrame_ = 0;

	// LOS(視線)判定のフレームカウンター
	int losTrueFrame_ = 0;
	int losFalseFrame_ = 0;

	float initialHP_ = 3.0f;
	int spawnParticleCount_ = 2;

	float stopChasingDistance_ = 15.0f;

	// path/LOS tuning
	float playerMoveRePathThreshold_ = 1.0f;
	float followPathToDirectDistance_ = 1.5f;
	float goalRePathDistance_ = 2.5f;
	int losTrueToDirectFrames_ = 10;
	float directReturnPathProgress_ = 0.7f;
	int losFalseToPathFrames_ = 1;

	int stuckToPathFrames_ = 10;
	int stuckFollowSkipWaypointFrames_ = 20;

	float waypointArriveDist_ = 1.0f;

	float directionEpsilon_ = 0.001f;
	float moveInterpolateRate_ = 0.2f;
	float noDirectionDamping_ = 0.9f;

	float stuckSideEpsilon_ = 0.0001f;
	float stuckSidePush_ = 0.05f;

	float stuckDetectMoveEpsilon_ = 0.05f;

	int walkParticleCount_ = 1;

	// attack tuning
	int attackBulletCount_ = 24;
	float bulletSpeed_ = 0.2f;
	float bulletSpawnYOffset_ = 0.5f;
	float attackCooldownSec_ = 0.0f;

	// bomb damage
	float timeBombExplosionDamage_ = 1.5f;

	// vignette effect
	float vignetteEmitHeight_ = 1.0f;
	int vignetteParticleCount_ = 3;
	uint32_t maxVignetteTimeFrames_ = 60 * 3;

};

