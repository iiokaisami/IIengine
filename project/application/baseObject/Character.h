#pragma once

#include "GameObject.h"
#include "../../gameEngine/collider/ColliderManager.h"

#include <memory>
#include <string>
#include <Object3d.h>
#include <Sprite.h>

/// <summary>
/// キャラクター基底クラス
/// </summary>
class Character : public GameObject
{
public:

	Character() = default;
	virtual ~Character() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelFileName">モデルファイル名</param>
	/// <param name="objectName">オブジェクト名</param>
	/// <param name="initialHP">初期HP</param>
	virtual void Initialize(const std::string& _modelFileName, const std::string& _objectName, float _initialHP);
	
	// 終了
	virtual void Finalize() override;

	// 更新
	virtual void Update() override;

	// 描画
	virtual void Draw() override;

	// スプライト描画
	virtual void Draw2D();

	// HPバー更新
	virtual void UpdateHPBar();

public: // ゲッター

	// HP取得
	virtual float   GetHP() const { return hp_; };
	// デスフラグ
	virtual bool IsDead() const { return isDead_; }
	

public: // セッター

	/// <summary>
	/// HP設定
	/// </summary>
	/// <param name="_hp">HP</param>
	virtual void SetHP(float _hp) { hp_ = _hp; };

	/// <summary>
	/// デスフラグ設定
	/// </summary>
	/// <param name="_isDead">デスフラグ</param>
	virtual void SetIsDead(bool _isDead) { isDead_ = _isDead; };

	

protected: // 動作処理

	/// <summary>
	/// 移動処理
	/// </summary>
	virtual void Move() {};

	/// <summary>
	/// 攻撃処理
	/// </summary>
	virtual void Attack() {};

	/// <summary>
	/// モデル変形の更新
	/// </summary>
	void UpdateModel();

protected: // 衝突判定

	/// <summary>
	/// 衝突判定時の処理
	/// </summary>
	/// <param name="other">衝突相手のコライダー</param>
	virtual void OnCollisionTrigger(const Collider* _other) { _other; }

	/// <summary>
    /// 衝突中の処理
    /// </summary>
    /// <param name="_other">衝突相手のコライダー</param>
	virtual void OnCollision(const Collider* _other) { _other; }

	/// <summary>
	/// 衝突時の押し出し処理
	/// </summary>
	/// <param name="otherAABB">相手のAABB</param>
	/// <param name="selfAABB">自分のAABB</param>
	/// <param name="position">自分の位置</param>
	void CorrectOverlap(const AABB& otherAABB, AABB& selfAABB, Vector3& position);

	/// <summary>
	/// AABBの重なり判定
	/// </summary>
	/// <param name="a">AABB a</param>
	/// <param name="b">AABB b</param>
	/// <returns>重なっているかどうか</returns>
	bool IsAABBOverlap(const AABB& a, const AABB& b);

protected:

	// 3D 描画用のオブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;

	// 衝突判定用
	ColliderManager* colliderManager_ = nullptr;
	Collider collider_;
	AABB aabb_;
	Collider::ColliderDesc colliderDesc_ = {};

	// デフォルトフレームレート
	static constexpr float kDefaultFrameRate = 60.0f;

	// 各種プロパティ
	Vector3 moveVelocity_{};       // 移動速度
	float moveSpeed_ = 0.1f;       // 移動スピード
	bool isInvincible_ = false;    // 無敵状態かどうか
	bool isHit_ = false;           // 被弾フラグ
	bool isFarFromPlayer_ = false; // プレイヤーとの距離フラグ

	// HPバー
	Vector2 hpBarSize_ = { 100.0f, 20.0f };
	float maxHP_ = 100.0f;
	Vector3 hpBarOffset_ = { 0.0f, 1.6f, 1.5f };
	float hpRatio_ = 1.0f;
	float hpLerpSpeed_ = 10.0f;

	// スプライト
	std::vector<std::unique_ptr<Sprite>> sprites_;
	uint32_t spriteNum_ = 1;

	// HP
	float hp_;
	// デスフラグ
	bool isDead_ = false;
	
};

