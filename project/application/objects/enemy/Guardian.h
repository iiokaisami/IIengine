#pragma once

#include "../../baseObject/BaseEnemy.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Framework.h>

/// <summary>
/// ガーディアン
/// ボス的な存在
/// </summary>
class Guardian : public BaseEnemy
{
public:

	Guardian() = default;
	~Guardian() = default;

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

public: // ゲッター

public: // セッター	

	// プレイヤーの位置をセット
	void SetPlayerPosition(const Vector3& _playerPosition) { playerPosition_ = _playerPosition; }


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

private:

	float initialHP_ = 30.0f;

	// パーティクル数
	int spawnParticleCount_ = 2;
	int moveParticleCount_ = 1;	

	// bomb ダメージ
	float timeBombExplosionDamage_ = 1.5f;

	// 移動関連
	float moveInterpolateRate_ = 0.1f;
	float moveVelocityDivisor_ = 7.0f;
	float playerMoveRePathThreshold_ = 1.0f;
	// 追尾停止距離
	const float kStopChasingDistance = 15.0f;

};

