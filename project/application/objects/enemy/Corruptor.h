#pragma once

#include "../../baseObject/GameObject.h"
#include "behaviorState/corruptorState/CorruptorBehaviorState.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Sprite.h>
#include <Framework.h>

/// <summary>
/// トラップエネミー死亡時に出現する敵
/// </summary>
class Corruptor : public GameObject
{
public:

	Corruptor() = default;
	~Corruptor() = default;
	
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

	/// <summary>
	/// 行動ステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステートのポインタ</param>
	void ChangeBehaviorState(std::unique_ptr<CorruptorBehaviorState> _pState);
	
	/// <summary>
	/// objectのtransformをセット
	/// </summary>
	/// <param name="_position">位置</param>
	/// <param name="_rotation">回転</param>
	/// <param name="_scale">スケール</param>
	void ObjectTransformSet(const Vector3& _position, const Vector3& _rotation, const Vector3& _scale);

private: // 衝突判定

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollisionTrigger(const Collider* _other);

public: // ゲッター

	// 被弾フラグ
	bool IsHit() const { return isHit_; }

	// プレイヤーとの距離が一定以上かどうかのフラグ
	bool IsFarFromPlayer() const { return isFarFromPlayer_; }

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

	/// <summary>
	/// 爆発したかどうかのフラグをセット
	/// </summary>
	/// <param name="_isExploded">爆発フラグ</param>
	void SetIsExploded(bool _isExploded) { isExploded_ = _isExploded; }

	// オブジェクトのpositionをセット
	void SetObjectPosition(const Vector3& _position) { object_->SetPosition(_position); }
	// オブジェクトのrotationをセット
	void SetObjectRotation(const Vector3& _rotation) { object_->SetRotate(_rotation); }
	// オブジェクトのscaleをセット
	void SetObjectScale(const Vector3& _scale) { object_->SetScale(_scale); }

private:

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

	// 行動ステート
	std::unique_ptr<CorruptorBehaviorState> pBehaviorState_ = nullptr;

	// 無敵フラグ(出現時等攻撃を受けなくさせる)
	bool isInvincible_ = true;

	// 被弾フラグ
	bool isHit_ = false;

	// プレイヤーとの距離が一定以上かどうかのフラグ
	bool isFarFromPlayer_ = false;

	// 爆発フラグ(爆発後の判定用にisDead_は取っておきたい)
	bool isExploded_ = false;

};

