#pragma once

#include "../../baseObject/GameObject.h"
#include "bullet/VignetteTrap.h"
#include "bullet/TimeBomb.h"
#include "behaviorState/trapEnemyState/TrapEnemyBehaviorState.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Sprite.h>
#include <Framework.h>

/// <summary>
/// 罠を設置してくる敵
/// </summary>
class TrapEnemy : public GameObject
{
public:

	TrapEnemy() = default;
	~TrapEnemy() = default;
	
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

	// 罠初期化
	void TrapInit();

	/// <summary>
	/// 行動ステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステートのポインタ</param>
	void ChangeBehaviorState(std::unique_ptr<TrapEnemyBehaviorState> _pState);

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

public: // ゲッター

	// 被弾フラグ
	bool IsHit() const { return isHit_; }
	// 罠設置のクールタイム完了フラグ
	bool IsTrapCooldownComplete() const { return isTrapCooldownComplete_; }
	// 罠を設置する距離かどうかのフラグ
	bool IsStopAndTrap() const { return isStopAndTrap_; }

	// 残っているTimeBombの座標を取得
	std::vector<Vector3> GetRemainingTimeBombPositions() const;

public: // セッター

	/// <summary>
	/// プレイヤーの位置をセット
	/// </summary>
	/// <param name="_playerPosition">プレイヤーの位置</param>
	Vector3 SetPlayerPosition(const Vector3& _playerPosition) { return playerPosition_ = _playerPosition; }

	/// <summary>
	/// 無敵フラグをセット
	/// </summary>
	/// <param name="_isInvincible">無敵フラグ</param>
	void SetIsInvincible(bool _isInvincible) { isInvincible_ = _isInvincible; }

	/// <summary>
	/// 被弾フラグをセット
	/// </summary>
	/// <param name="_isHit">被弾フラグ</param>
	void SetIsHit(bool _isHit) { isHit_ = _isHit; }

	// オブジェクトのpositionをセット
	void SetObjectPosition(const Vector3& _position) { object_->SetPosition(_position); }
	// オブジェクトのrotationをセット
	void SetObjectRotation(const Vector3& _rotation) { object_->SetRotate(_rotation); }
	// オブジェクトのscaleをセット
	void SetObjectScale(const Vector3& _scale) { object_->SetScale(_scale); }

	// 罠設置完了フラグセット
	void SetIsTrapCooldownComplete(bool _isTrapCooldownComplete) { isTrapCooldownComplete_ = _isTrapCooldownComplete; }
	// 次の罠がTimeBombかVignetteTrapかのフラグ反転
	void ChangeIsNextTrapTimeBomb() { isNextTrapTimeBomb_ = !isNextTrapTimeBomb_; }

	/// <summary>
	/// objectのtransformをセット
	/// </summary>
	/// <param name="_position">位置</param>
	/// <param name="_rotation">回転</param>
	/// <param name="_scale">スケール</param>
	void ObjectTransformSet(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale);

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
	const float kTooCloseDistance = 6.0f;
	// 追尾開始距離
	const float kTooFarDistance = 20.0f;

	// 罠
	std::vector<std::unique_ptr<TimeBomb>> pTimeBomb_ = {};
	std::vector<std::unique_ptr<VignetteTrap>> pVignetteTrap_ = {};
	
	// クールタイム完了フラグ
	bool isTrapCooldownComplete_ = false;
	// 次の罠がTimeBombかVignetteTrapかのフラグ
	bool isNextTrapTimeBomb_ = true;

	// 行動ステート
	std::unique_ptr<TrapEnemyBehaviorState> pBehaviorState_ = nullptr;

	// 離れる
	bool isEscape_ = false;
	// 近づく
	bool isApproach_ = false;
	// 罠を設置する
	bool isStopAndTrap_ = false;


	// 無敵フラグ(出現時等攻撃を受けなくさせる)
	bool isInvincible_ = true;

	// 被弾フラグ
	bool isHit_ = false;

};

