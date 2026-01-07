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
	// オブジェクトがアクティブかどうか(回避中など判定を付けたくない場合等)
	virtual bool IsActive() const
	{
		return isActive_;
	}

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
	/// オブジェクトがアクティブかどうか設定(回避中など判定を付けたくない場合等)
	/// </summary>
	/// <param name="_isActive">アクティブかどうか</param>
	virtual void SetIsActive(bool _isActive) { isActive_ = _isActive; };

protected: // メンバー

    std::string objectName_;
    Vector3 scale_;
    Vector3 rotation_;
    Vector3 position_;
	// オブジェクトがアクティブかどうか(回避中など判定を付けたくない場合等)
	bool isActive_ = false;
};