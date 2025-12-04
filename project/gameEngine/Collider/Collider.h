#pragma once

#include<vector>
#include<string>
#include<functional>
#include<list>
#include<variant>

#include"Shape.h"
#include"../../application/BaseObject/GameObject.h"

class ColliderManager;

/// <summary>
/// コライダー
/// 形状はSphere、OBB、AABBの3種類
/// </summary>
class Collider
{
public:

    Collider();
    ~Collider();

    /// <summary>
    /// コライダー作成の構造体
    /**
     * @brief コライダー作成用の構造体
     * owner: 所有するGameObject
     * colliderID: コライダー識別ID
     * shape: 形状タイプ（AABB/OBB/Sphere）
     * shapeData: 形状データへのポインタ
     * attribute: 衝突属性
     * onCollision: 衝突時コールバック
     * onCollisionTrigger: 衝突開始時コールバック
     */
    /// </summary>
    struct ColliderDesc
    {
        GameObject* owner = nullptr;
        std::string colliderID;
        Shape shape = Shape::AABB;
        void* shapeData = nullptr;
        uint32_t attribute = 0;
        std::function<void(const Collider*)> onCollision = nullptr;
        std::function<void(const Collider*)> onCollisionTrigger = nullptr;
    };

    /// <summary>
    /// AABBコライダー作成
    /// </summary>
    /// <param name="desc">コライダー作成用構造体</param>
    void MakeAABBDesc(const ColliderDesc& desc);

    /// <summary>
	/// OBBコライダー作成
    /// </summary>
    /// <param name="desc">コライダー作成用構造体</param>
    void MakeOBBDesc(const ColliderDesc& desc);

    /// <summary>
	/// Sphereコライダー作成
    /// </summary>
    /// <param name="desc">コライダー作成用構造体</param>
    void MakeSphereDesc(const ColliderDesc& desc);

public: // ゲッター

	// オーナー取得
    inline const GameObject* GetOwner()const { return owner_; }
	
	// 形状データ取得(AABB)
    inline const AABB* GetAABB()const { return std::get<AABB*>(shapeData_); }
    
	// 形状データ取得(OBB)
    inline const OBB* GetOBB()const { return std::get<OBB*>(shapeData_); }
    
	// 形状データ取得(Sphere)
    inline const Sphere* GetSphere()const { return std::get<Sphere*>(shapeData_); }

	// 衝突属性取得
    inline uint32_t GetCollisionAttribute()const { return collisionAttribute_; }
    
	// 衝突マスク取得
    inline uint32_t GetCollisionMask()const { return collisionMask_; }
    
	// 形状タイプ取得
    inline Shape GetShape()const { return shape_; }
    
	// 軽量化用半径取得
    inline unsigned int GetRadius() const { return radiusCollider_; }
    
	// コライダーID取得
    inline const std::string& GetColliderID() const { return colliderID_; }
	
    // 軽量化用有効フラグ取得
    inline bool GetIsEnableLighter() const { return enableLighter_; }
	
    // 軽量化用位置取得
    inline Vector3 GetPosition() const { return position_; }
	
    // 軽量化用有効フラグ取得
    inline const bool GetEnable() const { return isEnableCollision_; }

	// あたっているコライダーの中に指定されたポインタがあるか
    const bool IsRegisteredCollidingPtr(const Collider* _ptr) const;
	// あたっているコライダーリストから指定されたポインタを削除
    void EraseCollidingPtr(const Collider* _ptr);


public: // セッター

    /// <summary>
	/// オーナー設定
    /// </summary>
    /// <param name="_owner">GameObjectのポインタ</param>
    inline void SetOwner(GameObject* _owner) { owner_ = _owner; }
    
    /// <summary>
	/// コライダーID設定
    /// </summary>
    /// <param name="_id">コライダーID文字列</param>
    inline void SetColliderID(const std::string& _id)
    {
        colliderID_ = _id;
    }

    template<typename T>
   
	/// <summary>
	/// 形状データ設定
	/// </summary>
	/// <param name="_shapeData">形状データへのポインタ</param>
    void SetShapeData(T* _shapeData) { shapeData_ = _shapeData; }
   
    /// <summary>
	/// 形状タイプ設定
    /// </summary>
    /// <param name="_shape">形状データへのポインタ</param>
    void SetShape(Shape _shape) { shape_ = _shape; }

    /// <summary>
	/// 衝突属性設定
    /// </summary>
    /// <param name="_attribute">属性値</param>
    void SetAttribute(uint32_t _attribute);
   
    /// <summary>
	/// 衝突マスク設定
    /// </summary>
    /// <param name="_mask">マスク値へのポインタ</param>
    void SetMask(uint32_t _mask);
   
    /// <summary>
	/// 衝突時コールバック設定
    /// </summary>
    /// <param name="_func">コールバック関数</param>
    void SetOnCollision(const std::function<void(const Collider*)>& _func) { onCollisionFunction_ = _func; }
   
    /// <summary>
	/// 衝突開始時コールバック設定
    /// </summary>
    /// <param name="_func">コールバック関数</param>
    void SetOnCollisionTrigger(const std::function<void(const Collider*)>& _func) { onCollisionTriggerFunction_ = _func; }
    
    /// <summary>
	/// 軽量化用半径設定
    /// </summary>
	/// <param name="_rad">半径</param>
    void SetRadius(unsigned int _rad) { radiusCollider_ = _rad; }
    
    /// <summary>
	/// 軽量化用位置設定
    /// </summary>
	/// <param name="_v"> 位置ベクトル</param>
    void SetPosition(const Vector3& _v) { position_ = _v; }
    
	/// <summary>
	/// 軽量化用有効フラグ設定
	/// </summary>
	/// <param name="_flag">有効フラグ</param>
    void SetEnableLighter(bool _flag) { enableLighter_ = _flag; }
    
	/// <summary>
	/// コライダー有効フラグ設定
	/// </summary>
	/// <param name="_flag">有効フラグ</param>
    void SetEnable(bool _flag) { isEnableCollision_ = _flag; }
    
	/// <summary>
	/// あたっているコライダーリストに指定されたポインタを登録
	/// </summary>
	/// <param name="_ptr">コライダーポインタ</param>
    void RegisterCollidingPtr(const Collider* _ptr) { collidingPtrList_.push_back(_ptr); }

    /// <summary>
	/// 衝突時処理
    /// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
    inline void OnCollision(const Collider* _other)
    {
        if (onCollisionFunction_)
            onCollisionFunction_(_other);
        return;
    }

    /// <summary>
	/// 衝突開始時処理
    /// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
    void OnCollisionTrigger(const Collider* _other);

private:

    std::function<void(const Collider*)> onCollisionFunction_;
    std::function<void(const Collider*)> onCollisionTriggerFunction_;
    std::variant<OBB*, AABB*, Sphere*> shapeData_ = {};

    GameObject* owner_ = nullptr;
    bool isEnableCollision_ = true; // 判定をするかどうか
    Shape shape_ = Shape::Sphere; // 形状
    std::string colliderID_ = {}; // ID


    std::list<const Collider*> collidingPtrList_ = {}; // 現在あたっているコライダーのリスト

    // 軽量化用
    unsigned int radiusCollider_ = 0u;
    Vector3 position_ = {};
    bool enableLighter_ = false;

    // 衝突属性(自分)
    uint32_t collisionAttribute_ = 0xffffffff;
    // 衝突マスク(相手)
    uint32_t collisionMask_ = 0xffffffff;
};

