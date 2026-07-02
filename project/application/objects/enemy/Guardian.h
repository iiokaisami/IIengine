#pragma once

#include "../../baseObject/BaseEnemy.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

// 行動ステート
#include "behaviorState/guardianState/GuardianBehaviorState.h"
// 攻撃コントローラー
#include "bullet/GuardianAttackController.h"

#include "bullet/ShockWave.h"

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

	// ジャンプ開始
	void StartJump();

	// ジャンプ攻撃
	void JumpAttack();

	// ジャンプ上昇
	void UpdateJumpRise();

	// ジャンプ下降
	void UpdateJumpFall();

	// ジャンプ着地
	void UpdateJumpLanding();

	// 衝撃波生成
	void SpawnShockWave();
	
	/// <summary>
	/// 行動ステート切り替え
	/// </summary>
	/// <param name="_pState">新しいステートポインタ</param>
	void ChangeBehaviorState(std::unique_ptr<GuardianBehaviorState> _pState);

private: // 内部関数

	// 向きをプレイヤーに向ける
	void FaceToPlayer();

	// 攻撃パターン切り替え
	void SwitchAttackPattern(uint32_t _patternIndex);


public: // ゲッター

	// 攻撃がヒットしたかどうか
	bool IsHit() const { return isHit_; }

	// プレイヤーまでの距離を取得
	float GetDistanceToPlayer() const { return (playerPosition_ - position_).Length(); }

	// ジャンプ開始
	bool GetJumping() const { return isJumping_; }

	// ジャンプ着地フラグを取得
	float GetLanding() const { return isLanding_; }

	// ジャンプ開始HPを取得
	float GetJumpStartHP() const { return kJumpStartHP_; }

	// ジャンプ周期を取得
	float GetJumpCycle() const { return kJumpCycle_; }

	// 攻撃周期を取得
	float GetAttackCycle() const { return attackCycle_; }


public: // セッター	

	// プレイヤーの位置をセット
	void SetPlayerPosition(const Vector3& _playerPosition) { playerPosition_ = _playerPosition; }

	// 無敵状態の設定
	void SetInvincible(bool _isInvincible) { isInvincible_ = _isInvincible; }

	// ヒットフラグの設定
	void SetIsHit(bool _isHit) { isHit_ = _isHit; }

	// ジャンプ開始の設定
	void SetJumping(bool _isJumping) { isJumping_ = _isJumping; }

	// ジャンプ着地フラグの設定
	void SetLanding(bool _isLanding) { isLanding_ = _isLanding; }

	// --- ステートでのTransform操作用関数(引数あり) ---

	// オブジェクトの positionをセット
	void SetObjectPosition(const Vector3& _position) { object_->SetPosition(_position); }
	// オブジェクトの rotationをセット
	void SetObjectRotation(const Vector3& _rotation) { object_->SetRotate(_rotation); }
	// オブジェクトの scaleをセット
	void SetObjectScale(const Vector3& _scale) { object_->SetScale(_scale); }

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

	// 攻撃パターン切り替えの条件とインデックスのペア
	struct PatternCondition
	{
		std::function<bool(const Guardian&)> condition;
		size_t patternIndex;
	};

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

	// 攻撃切り替え距離
	const float kSpreadRange_ = 10.0f; // 近距離扇状
	const float kSpiralRange_ = 15.0f; // 中距離螺旋
	const float kLaserRange_ = 17.0f;  // 遠距離レーザー

	// ジャンプアタックの状態
	enum class JumpState
	{
		None,
		Rise,
		Fall,
		Landing
	};
	JumpState jumpState_ = JumpState::None;

	// ジャンプ開始HP
	const float kJumpStartHP_ = 18.0f;
	// ジャンプ周期
	const float kJumpCycle_ = 2.0f;
	// 攻撃周期
	float attackCycle_ = 0.0f;
	// ジャンプ開始
	bool isJumping_ = false;
	// ジャンプ開始時のY
	float jumpStartY_ = 0.0f;
	// ジャンプ最大高度
	const float jumpMaxHeight_ = 8.0f;
	// ジャンプ上昇速度
	float ascendSpeed_ = 8.0f;
	// ジャンプ下降速度
	float descendSpeed_ = 25.0f;
	// ジャンプ着地時のフラグ
	bool isLanding_ = false;

	// Guardian
	std::unique_ptr<ShockWave> pShockWave_;

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

