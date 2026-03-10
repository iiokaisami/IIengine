#pragma once

#include "../../baseObject/BaseEnemy.h"
#include "behaviorState/corruptorState/CorruptorBehaviorState.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Sprite.h>
#include <Framework.h>

/// <summary>
/// トラップエネミー死亡時に出現する敵
/// </summary>
class Corruptor : public BaseEnemy
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
	
	// ImGui
	void ImGuiDraw();

	// 移動
	void Move() override;

	/// <summary>
	/// 行動ステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステートのポインタ</param>
	void ChangeBehaviorState(std::unique_ptr<CorruptorBehaviorState> _pState);

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

	
	// --- ステートでのTransform操作用関数(引数あり) ---
	// オブジェクトの positionをセット
	void SetObjectPosition(const Vector3& _position) { object_->SetPosition(_position); }
	// オブジェクトの rotationをセット
	void SetObjectRotation(const Vector3& _rotation) { object_->SetRotate(_rotation); }
	// オブジェクトの scaleをセット
	void SetObjectScale(const Vector3& _scale) { object_->SetScale(_scale); }

private:

	// 行動ステート
	std::unique_ptr<CorruptorBehaviorState> pBehaviorState_ = nullptr;

	// 無敵フラグ(出現時等攻撃を受けなくさせる)
	bool isInvincible_ = true;

	// 被弾フラグ
	bool isHit_ = false;

	// プレイヤーとの距離が一定以上かどうかのフラグ
	bool isFarFromPlayer_ = false;

	// 爆発フラグ
	bool isExploded_ = false;

	float initialHP_ = 1.0f;
	Vector3 initialScale_ = { 1.0f, 1.0f, 1.0f };

	int spawnParticleCount_ = 2;

	// isFarFromPlayer_ 判定に使う閾値
	float nearPlayerDistance_ = 2.5f;
	// 爆発後スケール増分
	float explodedScaleGrowPerFrame_ = 0.1f;      

	// 移動関連
	float moveInterpolateRate_ = 0.1f;
	float moveVelocityDivisor_ = 7.0f;
	int moveParticleCount_ = 1;

	// 障害物回避関連
	float separationDistance_ = 2.5f;
	float separationPush_ = 0.1f;

};

