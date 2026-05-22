#pragma once

#include "../../baseObject/BaseEnemy.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

// 行動ステート
#include "behaviorState/guardianState/GuardianBehaviorState.h"
// 攻撃コントローラー
#include "bullet/GuardianAttackController.h"

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

	// 攻撃
	void Attack() override;

	/// <summary>
	/// 行動ステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステートポインタ</param>
	void ChangeBehaviorState(std::unique_ptr<GuardianBehaviorState> _pState);

public: // ゲッター

public: // セッター	

	// プレイヤーの位置をセット
	void SetPlayerPosition(const Vector3& _playerPosition) { playerPosition_ = _playerPosition; }

	// 無敵状態の設定
	void SetInvincible(bool _isInvincible) { isInvincible_ = _isInvincible; }

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

	// 行動ステート
	std::unique_ptr<GuardianBehaviorState> pBehaviorState_ = nullptr;

	float initialHP_ = 30.0f;

	// 攻撃コントローラー
	GuardianAttackController attackController_;
	// 弾の管理クラス
	BulletManager bulletManager_;

	// 確認用変数
	size_t currentPatternIndex_ = 0;       // 現在のパターン
	float patternInterval_ = 3.0f;         // パターン切り替え間隔[秒]
	float patternTimer_ = 0.0f;            // 経過タイマー
	float shootInterval_ = 3.0f;           // 発射間隔[秒]
	float shootTimer_ = 0.0f;              // 次弾までのタイマー

	// 各弾幕のパラメータ
	// 扇状弾幕
	uint32_t spreadCount_ = 5;
	float spreadAngle_ = 60.0f;
	float spreadBulletSpeed_ = 0.22f;
	// 螺旋弾幕
	float spiralAngleSpeed_ = 3.5f;
	float spiralBulletSpeed_ = 0.18f;
	uint32_t spiralShotsFired_ = 0;
	uint32_t spiralMaxShots_ = 30;
	// レーザー 
	Vector3 laserDirection_ = { 0.0f, 0.0f, 1.0f };
	float laserSpeed_ = 0.5f;

	// パーティクル数
	int spawnParticleCount_ = 2;
	int moveParticleCount_ = 1;	

	// bomb ダメージ
	float timeBombExplosionDamage_ = 1.5f;

	// 移動関連
	float moveInterpolateRate_ = 0.1f;
	float moveVelocityDivisor_ = 12.0f; // 大きいほど移動速度が遅くなる
	float playerMoveRePathThreshold_ = 1.0f;
	// 追尾停止距離
	const float kStopChasingDistance = 15.0f;

};

