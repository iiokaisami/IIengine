#pragma once

#include <Vector3.h>
#include <string>

#include "../../gameEngine/Collider/Shape.h"

/// <summary>
/// ゲームオブジェクトの基底クラス
/// </summary>
class GameObject
{
public:

    GameObject();
    virtual ~GameObject() {};

public: // 仮想関数

	// 初期化
    virtual void Initialize() = 0;
	// 更新
    virtual void Update() = 0;
	// 描画
    virtual void Draw() = 0;
	// 終了
    virtual void Finalize() = 0;

public: // ゲッター

	// スケール取得
    virtual Vector3 GetScale() const { return scale_; };
	// 回転取得
    virtual Vector3 GetRotation() const { return rotation_; };
	// 位置取得
    virtual Vector3 GetPosition() const { return position_; };
	// HP取得
    virtual float   GetHP() const { return hp_; };
	// デスフラグ
    virtual bool IsDead() const { return isDead_; }
	// オブジェクトがアクティブかどうか(回避中など判定を付けたくない場合等)
    virtual bool IsActive() const { return isActive_; }

public: // セッター

	/// <summary>
	/// スケール設定
	/// </summary>
	/// <param name="_scale">スケール</param>
	virtual void SetScale(const Vector3& _scale) { scale_ = _scale; };
    
	/// <summary>
	/// 回転設定
	/// </summary>
	/// <param name="_rotation">回転</param>
	virtual void SetRotation(const Vector3& _rotation) { rotation_ = _rotation; };
    
	/// <summary>
	/// 位置設定
	/// </summary>
	/// <param name="_position">位置</param>
	virtual void SetPosition(const Vector3& _position) { position_ = _position; };
    
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
	
	/// <summary>
	/// オブジェクトがアクティブかどうか設定(回避中など判定を付けたくない場合等)
	/// </summary>
	/// <param name="_isActive">アクティブかどうか</param>
    virtual void SetIsActive(bool _isActive) { isActive_ = _isActive; };


protected: // メンバー

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

    std::string objectName_;
    Vector3 scale_;
    Vector3 rotation_;
    Vector3 position_;
    float hp_;
    // デスフラグ
	bool isDead_ = false; 
    // オブジェクトがアクティブかどうか(回避中など判定を付けたくない場合等)
	bool isActive_ = false;
};