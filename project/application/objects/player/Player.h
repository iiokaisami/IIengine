#pragma once

#include "../../baseObject/Character.h"
#include "bullet/PlayerBullet.h"
#include"../../../gameEngine/collider/ColliderManager.h"

#include "postEffect/PostEffectManager.h"


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

private: // 内部処理

	// 死亡演出更新
	bool UpdateDeathMotion();

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

	// 死亡演出
	void DeadEffect();

	// オート移動
	void AutoMove();

	// オート攻撃
	void AutoAttack();

	// 移動制限
	void ClampPosition();

	// 回避スロー
	void EvadeSlow();

	// ダメージRGBShit
	void DamageRGBShift();

	// 回避コンボエフェクト
	void EvadeComboInversion();

	// HP減少ノイズ
	void HPDecreaseNoise();

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

	// ヒットした瞬間のフラグのゲッター
	bool IsHitMoment() const { return isHitMoment_; }

	// 移動速度のゲッター
	Vector3 GetVelocity() const { return moveVelocity_; }

	// オートフラグのゲッター
	bool IsAutoControl() const { return isAutoControl_; }

	// 死亡演出がアクティブかどうか
	bool IsDeathMotionComplete() const { return deathMotion_.isComplete; }

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
	void SetAutoControl(bool _isAuto) { isAutoControl_ = _isAuto; }

	/// <summary>
	/// 移動可能フラグのセッター
	/// </summary>
	/// <param name="_isCanMove">移動可能フラグ</param>
	bool SetIsCanMove(bool _isCanMove) { return isCanMove_ = _isCanMove; }

	/// <summary>
	/// ターゲットエネミーの位置設定
	/// </summary>
	/// <param name="_targetPos">ターゲットエネミーの位置ベクトル</param>
	void SetTargetEnemyPosition(const Vector3& _targetPos) { targetEnemyPosition_ = _targetPos; hasTarget_ = true; }

	/// <summary>
	/// ゴールに触れたかのフラグのセッター
	/// </summary>
	/// <param name="_isTouchGoal">ゴールに触れたかのフラグ</param>
	void SetTouchGoal(bool _isTouchGoal) { isTouchGoal_ = _isTouchGoal; }

	/// <summary>
	/// ノイズをアクティブにするかのセッター
	/// </summary>
	/// <param name="_isNoiseActive">ノイズをアクティブにするかのフラグ</param>
	void SetIsNoiseActive(bool _isNoiseActive) { isNoiseActive_ = _isNoiseActive; }

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

	// タイトル用オートフラグ(trueだったら自動で動く 入力は受け付けない)
	bool isAutoControl_ = false;

	// ヒットした瞬間のフラグ
	bool isHitMoment_ = false;

	// ゴールに触れたかのフラグ
	bool isTouchGoal_ = false;

	// 暗闇トラップに当たったかどうか
	bool isHitVignetteTrap_ = false;
	// 暗闇を徐々に戻すフラグ
	bool isFadingOut_ = false;
	// 暗闇効果最大時間
	static constexpr float kMaxVignetteSec_ = (60.0f * 3.0f) / kDefaultFrameRate;
	// 暗闇タイマー
	float vignetteRemainingSec_ = kMaxVignetteSec_;
	// vignetteの強さ
	float vignetteStrength_ = 0.0f;


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

	// スローフラグ
	bool isSlowMotion_ = false;
	// Maxスロータイマー
	const float kSlowDurationSec_ = 2.45f;
	// 完全停止時間
	const float kSlowHoldTime_ = 0.18f;
	// 復帰時間
	const float kSlowRecoverTime_ = 0.15f;
	// スロータイマー
	float slowTimerSec_ = kSlowHoldTime_ + kSlowRecoverTime_;
	// スローの強さ
	float slowDuration_ = 0.4f;
	// ブラーの強さ
	float radialStrength_ = 0.0f;

	// RGBShiftタイマー
	float rgbShiftTimer_ = 0.0f;
	// RGBShift持続時間
	float rgbShiftDuration_ = 0.35f;
	// RGBShiftがアクティブかどうか
	bool  isRGBShiftActive_ = false;

	// ノイズの強さ
	float finalStrength_ = 0.0f;
	// ノイズの処理を施すかのフラグ
	bool isNoiseActive_ = false;

	// デスモーション用構造体
	struct Motion
	{
		bool isActive = false;
		bool isComplete = false;
		float timerSec = 0.0f;           
		float shakeSec = 40.0f;                              // ぷるぷる継続フレーム数
		// 振動パラメータ
		float wobbleAmplitude = 0.10f;                       // 振幅
		float wobbleFreqHz = 10.0f * kDefaultFrameRate;      // 周波数
		// 保存しておく基底トランスフォーム
		Vector3 startPosition = { 0.0f, 0.0f, 0.0f };
		Vector3 startRotation = { 0.0f, 0.0f, 0.0f };
		Vector3 startScale = { 1.0f, 1.0f, 1.0f };
		// pop スケール（はじける最大値、ただし今回は瞬時消失でも可）
		float popScale = 2.0f;
	};

	Motion deathMotion_;

	// クリアシーン用構造体
	struct ClearMotion
	{
		Vector3 center = { 0.0f, 0.0f, 0.0f };
		float radius = 3.0f;
		float angleSpeed = 1.5f;
		float currentAngle = 0.0f;
		float initialAngle = 0.0f;
		bool clockwise = true;
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};

	ClearMotion clearMotion_;
	
	// 回転の線形補間率
	float turnLerpRate_ = 0.2f;

	// オート攻撃の間隔
	float autoAttackIntervalSec_ = 0.8f;

	// クリアシーンの軌道パラメータ
	Vector3 clearOrbitCenter_ = { 0.0f, 0.5f, 0.0f };
	float clearOrbitRadius_ = 8.0f;
	float clearOrbitAngularSpeed_ = 1.5f;
	float clearOrbitInitialAngle_ = 0.2f;
	bool clearOrbitClockwise_ = false;

	float clearPoyoAmpX_ = 0.2f;
	float clearPoyoAmpY_ = 0.2f;
	float clearPoyoAmpZ_ = 0.2f;
	float clearPoyoFreqXZ_ = 1.2f;
	float clearPoyoFreqY_ = 1.2f;
	bool clearMaintainArea_ = false;
	bool clearMaintainVolume_ = false;
	Vector3 clearBaseScale_ = { 1.0f, 1.0f, 1.0f };
	float clearScaleMin_ = 0.05f;
	float clearScaleMax_ = 5.0f;
	int clearWalkParticleCount_ = 1;

	// ダメージエフェクトのパラメータ
	float rgbShiftAmplitude_ = 0.25f;
	float rgbShiftOscillation_ = 40.0f;
	float rgbShiftDamping_ = 8.0f;

	float hpDangerThreshold_ = 0.3f;
	float hpNoisePow_ = 2.5f;
	float hpNoisePulseFreq_ = 12.0f;
	float hpNoisePulseAmp_ = 0.15f;
	float hpNoiseBaseGain_ = 2.5f;

	// 死亡演出のパラメータ
	float deathHpThreshold_ = 0.3f;
	int hitParticleCountNormal_ = 5;
	int hitParticleCountHeavy_ = 8;
	float explosionDamage_ = 1.5f;

	// オート移動のパラメータ
	float autoMoveMinX_ = -15.0f, autoMoveMaxX_ = 15.0f;
	float autoMoveMinZ_ = -15.0f, autoMoveMaxZ_ = 15.0f;
	float autoMoveDirChangeMinSec_ = 1.0f;
	float autoMoveDirChangeMaxSec_ = 2.5f;
	float autoMoveMinDirLen_ = 0.1f;

	// オート攻撃のパラメータ
	float clearTotalTime_ = 0.0f;
	float clearCurrentAngle_ = 0.2f;
	float clearLastYaw_ = 0.0f;

};