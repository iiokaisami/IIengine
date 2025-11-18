#pragma once

#include "../../baseObject/GameObject.h"
#include "bullet/PlayerBullet.h"
#include"../../../gameEngine/collider/ColliderManager.h"

#include "postEffect/PostEffectManager.h"

#include <Object3d.h>
#include <Sprite.h>
#include <Framework.h>

/// <summary>
/// プレイヤークラス
/// プレイヤーの移動、攻撃、回避、当たり判定などを管理
/// </summary>
class Player : public GameObject
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
	
	// スプライト描画
	void Draw2D();
	
	// ImGui
	void ImGuiDraw();
	
	// 移動
	void Move();
	
	// 攻撃
	void Attack();

	// 回避
	void Evade();

	// 死亡演出
	void DeadEffect();

	/// <summary>
	/// デスモーション開始
	/// 外部から呼び出す用
	/// </summary>
	void StartDeathMotion();

	/// <summary>
	/// クリアシーン更新
	/// </summary>
	void ClearSceneUpdate();

private:

	// オート移動
	void AutoMove();

	// オート攻撃
	void AutoAttack();

	// 移動制限
	void ClampPosition();

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

private:

	// 3Dオブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;

	// 当たり判定関係
	ColliderManager* colliderManager_ = nullptr;
	Collider collider_;
	AABB aabb_;
	Collider::ColliderDesc desc = {};

	// 弾
	std::list<PlayerBullet*> pBullets_ = {};

	// 発射クールタイム
	const int kShootCoolDownFrame_ = 15;
	// 弾のクールタイム
	int countCoolDownFrame_ = 0;

	// 移動速度
	Vector3 moveVelocity_{};
	Vector3 moveSpeed_ = { 0.1f,0.0f,0.1f };

	// タイトル用オートフラグ(trueだったら自動で動く 入力は受け付けない)
	bool isAutoControl_ = false;

	// ヒットした瞬間のフラグ
	bool isHitMoment_ = false;

	// 暗闇トラップに当たったかどうか
	bool isHitVignetteTrap_ = false;
	// 暗闇を徐々に戻すフラグ
	bool isFadingOut_ = false;
	// 暗闇効果最大時間
	const uint32_t kMaxVignetteTime = 60 * 3;
	// 暗闇タイマー
	uint32_t vignetteTime_ = kMaxVignetteTime;
	// vignetteの強さ
	float vignetteStrength_ = 0.0f;


	// 回避フラグ
	bool isEvading_ = false;
	// 回避時間
	uint32_t evadeTime_ = 0;
	// 回避時間の最大値
	const uint32_t kEvadeTimeMax_ = 30;
	// 回避速度
	Vector3 evadeSpeed_ = { 0.2f,0.0f,0.2f };
	// 回避方向
	Vector3 evadeDirection_ = { 0.0f,0.0f,0.0f };
	// 回避中のフレーム数
	int32_t evadeFrame_ = 0;
	const int kEvadeDuration_ = 30; // 回避の持続時間
	// 回避速度
	const float kEvadeSpeed_ = 0.2f;

	float evadeStartRotationX_ = 0.0f; // 回避開始時のx軸角度
	float evadeTargetRotationX_ = 0.0f; // 回避中の目標x軸角度
	const float kEvadeRotateAngle_ = 3.14f * 4.0f; // 1回転(360度) 


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

	// デスモーション用構造体
	struct Motion
	{
		bool isActive = false;
		bool isComplete = false;
		uint32_t count = 0;           // 現在フレームカウント
		uint32_t shakeFrames = 40;    // ぷるぷる継続フレーム数（調整可）
		// 振動パラメータ
		float wobbleAmplitude = 0.10f; // 振幅
		float wobbleFreq = 10.0f;      // 周波数（frameベース）
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
	
};

