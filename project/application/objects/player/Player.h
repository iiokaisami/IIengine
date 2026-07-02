#pragma once

#include "../../baseObject/Character.h"
#include "bullet/PlayerBullet.h"
#include"../../../gameEngine/collider/ColliderManager.h"
#include "postEffect/PostEffectManager.h"

#include "motion/PlayerDeathMotion.h"
#include "motion/PlayerClearMotion.h"
#include "motion/PlayerAutoMotion.h"
#include "PlayerEffects.h"

#include "Quaternion.h"
#include <Object3d.h>
#include <Sprite.h>
#include <Framework.h>

/// <summary>
/// プレイヤークラス
/// プレイヤーの移動、攻撃、回避、当たり判定などを管理
/// </summary>
class Player : public Character
{
public:

	Player() = default;
	~Player() = default;
	
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

	// ターゲットエネミークリア
	void ClearTargetEnemy();
	
	/// <summary>
	/// デスモーション開始
	/// 外部から呼び出す用
	/// </summary>
	void StartDeathMotion();

	/// <summary>
	/// クリアシーン更新
	/// </summary>
	void ClearSceneUpdate();


	/// <summary>
	/// 回転更新
	/// </summary>
	/// <param name="_toAngle">向きたい方向ベクトル</param>
	void RotationUpdate(const Vector3& _toAngle);

	// movement/combat最低限
	const Vector3& GetMoveSpeed() const { return moveSpeed_; }
	void SetVelocity(const Vector3& v) { moveVelocity_ = v; }

	// 弾生成だけを担当する関数
	void FireBulletForward();

private: // 内部処理

	// 操作更新
	void UpdateControl();

	// ステータス更新
	void UpdateStatus();

	// 移動
	void Move() override;
	
	// 攻撃
	void Attack() override;

	// 回避
	void Evade();

	// 最も近い敵インジケーター更新
	void UpdateNearEnemyIndicator();

	// 移動制限
	void ClampPosition();

	// 回避コンボエフェクト
	void EvadeComboInversion();

	// ロックオン
	void LockOn();

	// タイトルデモで死んでいるときに復活させる
	void ReviveIfDeadInTitleDemo();


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

	// ヒットした瞬間のフラグのゲッター
	bool IsHitMoment() const { return isHitMoment_; }

	// 移動速度のゲッター
	Vector3 GetVelocity() const { return moveVelocity_; }

	// オートフラグのゲッター
	bool IsAutoControl() const { return autoMotion_.IsActive(); }

	// 死亡演出がアクティブかどうか
	bool IsDeathMotionComplete() const { return deathMotion_.IsComplete(); }

	// 最大HPのゲッター
	float GetMaxHP() const { return maxHP_; }

public: // セッター

	/// <summary>
	/// ヒットした瞬間のフラグのセッター
	/// </summary>
	/// <param name="_isHitMoment">ヒットした瞬間のフラグ</param>
	void SetHitMoment(bool _isHitMoment) { isHitMoment_ = _isHitMoment; }

	/// <summary>
	/// オートフラグのセッター
	/// </summary>
	/// <param name="_isAuto">オートフラグ</param>
	void SetAutoControl(bool _enable);

	/// <summary>
	/// 移動可能フラグのセッター
	/// </summary>
	/// <param name="_isCanMove">移動可能フラグ</param>
	bool SetIsCanMove(bool _isCanMove) { return isCanMove_ = _isCanMove; }

	/// <summary>
	/// ターゲットエネミーの位置設定
	/// </summary>
	/// <param name="_targetPos">ターゲットエネミーの位置ベクトル</param>
	void SetTargetEnemyPosition(const Vector3& _targetPos) { targetEnemyPosition_ = _targetPos; lockOnTargetPosition_ = _targetPos; hasTarget_ = true; }

	/// <summary>
	/// ゴールに触れたかのフラグのセッター
	/// </summary>
	/// <param name="_isTouchGoal">ゴールに触れたかのフラグ</param>
	void SetTouchGoal(bool _isTouchGoal) { isTouchGoal_ = _isTouchGoal; }

	/// <summary>
	/// ノイズをアクティブにするかのセッター
	/// </summary>
	/// <param name="_isNoiseActive">ノイズをアクティブにするかのフラグ</param>
	void SetIsNoiseActive(bool _isNoiseActive) { effects_.SetNoiseEnabled(_isNoiseActive); }

	/// <summary>
	/// バリアが破壊されているかのフラグのセッター
	/// </summary>
	/// <param name="_isBarrierBroken">バリアが破壊されているかのフラグ</param>
	void SetIsBarrierBroken(bool _isBarrierBroken) { isBarrierBroken_ = _isBarrierBroken; }

private:

	// 最も近い敵
	Vector3 targetEnemyPosition_ = { 0.0f,0.0f,0.0f };
	bool hasTarget_ = false;
	float indicatorSpinAngle_ = 0.0f;
	float indicatorSpinSpeed_ = 2.0f;
	float indicatorRadius_ = 60.0f;
	float indicatorMaxDetectRange_ = 400.0f;

	// 弾
	std::vector<std::unique_ptr<PlayerBullet>> pBullets_ = {};

	// 発射クールタイム
	static constexpr float kShootCoolDownSec_ = 15.0f / kDefaultFrameRate;
	// 弾のクールタイム
	float shootCooldownSec_ = 0.0f;

	// 移動速度
	Vector3 moveVelocity_{};
	Vector3 moveSpeed_ = { 0.1f,0.0f,0.1f };

	// デフォルト位置
	Vector3 defaultPosition_ = { 0.2f,0.7f,-1.2f };

	// ヒットした瞬間のフラグ
	bool isHitMoment_ = false;

	// ゴールに触れたかのフラグ
	bool isTouchGoal_ = false;

	// 回避フラグ
	bool isEvading_ = false;
	// 回避時間
	uint32_t evadeTime_ = 0;
	// 回避時間の最大値
	static constexpr float kEvadeDurationSec_ = 30.0f / kDefaultFrameRate;
	// 回避速度
	Vector3 evadeSpeed_ = { 0.2f,0.0f,0.2f };
	// 回避方向
	Vector3 evadeDirection_ = { 0.0f,0.0f,0.0f };
	// 回避中の残り時間
	float evadeRemainingSec_ = 0.0f;
	const int kEvadeDuration_ = 30; // 回避の持続時間
	// 回避速度
	const float kEvadeSpeed_ = 0.2f;
	// 回避クールタイム	
	static constexpr float kEvadeCoolDownSec_ = 1.0f;
	// 回避クールタイムの残り時間
	float evadeCooldownSec_ = 0.0f;

	float evadeStartRotationY_ = 0.0f; // 回避開始時のY軸角度
	float evadeTargetRotationY_ = 0.0f; // 回避中の目標Y軸角度
	const float kEvadeRotateAngle_ = 3.14f * 4.0f; // 1回転(360度) 

	// 回避コンボ用フラグ
	bool isEvadeComboActive_ = false;
	// この回避中に既にスローが発生したか
	bool hasEvadeSlowHit_ = false;
	// この回避中の inversion時間
	float inversionDuration_ = 0.2f;
	// inversion の残り時間
	float inversionRemainingSec_ = 0.0f;
	// 回避中のコンボ用：1回目ヒットからの経過時間
	float evadeComboWindowSec_ = 0.0f;
	// 同フレーム/同時ヒットを弾くための最低間隔
	static constexpr float kEvadeComboMinIntervalSec_ = 0.08f;

	// 環境マップ
	std::string cubeMapPath_ = "";
	uint32_t cubeSrvIndex_ = 0u;
	D3D12_GPU_DESCRIPTOR_HANDLE cubeHandle_ = { 0 };
	float environmentStrength_ = 1.0f;

	// 移動可能フラグ
	bool isCanMove_ = true;

	// 移動制限
	Vector2 limitMax_ = { 40.0f, 30.0f };
	Vector2 limitMin_ = { -40.0f, -30.0f };


	// ロックオンの有無
	bool isLockOn_ = false;
	// ロックオンのターゲット位置
	Vector3 lockOnTargetPosition_{};
	// ロックオンのチャージ範囲
	float lockOnChargeRange_ = 17.0f;
	// ロックオンのチャージ時間
	const float kLockOnChargeMaxTimeSec_ = 2.0f;
	// ロックオンのチャージタイマー
	float lockOnChargeTimerSec_ = 0.0f;
	// 前フレームのターゲットの位置
	Vector3 preTargetPos_{};
	// ロックオンスプライトのサイズ
	Vector2 lockOnIndicatorSize_ = { 100.0f, 100.0f };
	// ロックオンスプライトの回転角度
	float lockOnIndicatorAngle_ = 0.0f;
	// ロックオンスプライトの座標
	Vector2 lockOnIndicatorPos_{};
	// ロックオンスプライトの透明度
	float lockOnIndicatorAlpha_ = 0.0f;
	// ロックオンスプライトの色
	Vector4 lockOnIndicatorColor_ = { 1.0f, 0.1f, 0.1f,lockOnIndicatorAlpha_ };
	// ロックオンインジケーターのYオフセット
	float lockOnIndicatorOffsetY_ = -20.0f;

	// バリア破壊フラグ
	bool isBarrierBroken_ = false;

	// 各動きの呼び出し
	PlayerDeathMotion deathMotion_;
	PlayerClearMotion clearMotion_;
	PlayerAutoMotion  autoMotion_;
	
	// 各種エフェクト
	PlayerEffects effects_;

	// 回転の線形補間率
	float turnLerpRate_ = 0.2f;

	// 死亡演出のパラメータ
	float deathHpThreshold_ = 0.3f;
	int hitParticleCountNormal_ = 5;
	int hitParticleCountHeavy_ = 8;
	float explosionDamage_ = 1.5f;
	float shockwaveDamage_ = 2.0f;

};