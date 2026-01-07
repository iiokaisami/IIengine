#pragma once

#include "GameObject.h"
#include "../../gameEngine/collider/ColliderManager.h"

#include <memory>
#include <Object3d.h>


/// <summary>
/// 静的オブジェクト基底クラス
/// </summary>
class StaticObject : public GameObject
{
public:

    StaticObject() = default;
    virtual ~StaticObject() = default;

	// 初期化
    virtual void Initialize(const std::string& _modelFileName,const std::string& _objectName);
    
	// 終了
    virtual void Finalize() override;
    
	// 更新
    virtual void Update() override;
    
	// 描画
    virtual void Draw() override;

protected:

    // 衝突処理
    virtual void OnCollisionTrigger(const Collider* _other) { _other; }

protected:

    // 描画用の3Dオブジェクト
    std::unique_ptr<Object3d> object_ = nullptr;   
    // 衝突判定管理
    ColliderManager* colliderManager_ = nullptr; 
    // 衝突判定データ
    Collider collider_;                          
    // 衝突範囲
    AABB aabb_;                                   

};

