#pragma once

#include<vector>
#include<string>
#include<utility>
#include <list>

#include"Shape.h"
#include"Collider.h"

namespace IIEngine
{

    /// <summary>
    /// コライダー管理クラス
    /// </summary>
    class ColliderManager
    {

    public:

        ColliderManager(const ColliderManager&) = delete;
        ColliderManager(const ColliderManager&&) = delete;
        ColliderManager& operator=(const ColliderManager&) = delete;
        ColliderManager& operator=(const ColliderManager&&) = delete;

        static ColliderManager* GetInstance() { static ColliderManager instance; return &instance; }

        /// <summary>
        /// 初期化
        /// </summary>
        void Initialize();

        /// <summary>
        /// 全てのコリジョンチェック
        /// </summary>
        void CheckAllCollision();

        /// <summary>
        /// コライダー登録
        /// </summary>
        /// <param name="_collider"> 登録するコライダーのポインタ</param>
        void RegisterCollider(Collider* _collider);

        /// <summary>
        /// コライダーリストクリア
        /// </summary>
        void ClearColliderList();

        /// <summary>
        /// コライダー削除
        /// </summary>
        void DeleteCollider(Collider* _collider);

    public: // ゲッター

        /// <summary>
        /// 新しい属性値を取得
        /// </summary>
        /// <param name="_nameA"> 衝突判定名A</param>
        uint32_t GetNewAttribute(std::string _id);

        template <typename... Args>
        /// <summary>
        /// 新しいマスク値を取得
        /// </summary>
        /// <param name="_id"> 衝突判定名A</param>
        /// <param name="_ignoreNames"> 無視する属性名リスト</param>
        uint32_t* GetNewMask(std::string _id, Args... _ignoreNames)
        {
            uint32_t result = 0;
            for (auto& attributePair : attributeList_)
            {
                if (_id.compare(attributePair.first) == 0)
                {
                    result = ~attributePair.second;
                    break;
                }
            }
            for (std::string name : std::initializer_list<std::string>{ _ignoreNames... })
            {
                bool isCompare = false;
                for (auto& attributePair : attributeList_)
                {
                    if (name.compare(attributePair.first) == 0)
                    {
                        result ^= attributePair.second;
                        isCompare = true;
                    }
                }
            }

            if (!result) result = ~result;

            uint32_t* resultPtr = nullptr;
            bool isCompare = false;
            /// 配列の中身を変更
            for (auto& maskPair : maskList_)
            {
                if (maskPair.first.compare(_id) == 0)
                {
                    maskPair.second = result;
                    resultPtr = &maskPair.second;
                    isCompare = true;
                }
            }
            if (!isCompare) // マスクリストに登録されていなかったら
            {
                maskList_.push_back({ _id, result });
                resultPtr = &maskList_.back().second;
            }

            return resultPtr;
        }


    private:

        ColliderManager() = default;

        /// <summary>
        /// コリジョンペアチェック
        /// </summary>
        /// <param name="_colA"> コライダーA</param>
        /// <param name="_colB"> コライダーB</param>
        void CheckCollisionPair(Collider* _colA, Collider* _colB);

        /// <summary>
        /// 形状を軸に投影
        /// </summary>
        /// <param name="_v"> 形状の頂点リスト</param>
        /// <param name="_axis"> 投影する軸</param>
        /// <param name="_min"> 投影後の最小値</param>
        /// <param name="_max"> 投影後の最大値</param>
        void ProjectShapeOnAxis(const std::vector<Vector3>* _v, const Vector3& _axis, float& _min, float& _max);

        /// <summary>
        /// AABB同士の当たり判定
        /// </summary>
        /// <param name="_aabb1"> AABB1</param>
        /// <param name="_aabb2"> AABB2</param>
        /// <returns> 当たっているか</returns>
        bool IsCollision(const AABB* _aabb1, const AABB* _aabb2);
        /// <summary>
        /// AABBとSphereの当たり判定
        /// </summary>
        /// <param name="_aabb"> AABB</param>
        /// <param name="_sphere"> Sphere</param>
        /// <returns> 当たっているか</returns>
        bool IsCollision(const AABB& _aabb, const Sphere& _sphere);

        /// <summary>
        /// Sphere同士の当たり判定
        /// </summary>
        /// <param name="_sphere1"> Sphere1</param>
        /// <param name="_sphere2"> Sphere2</param>
        /// <returns> 当たっているか</returns>
        bool IsCollision(const Sphere* _sphere1, const Sphere* _sphere2);
        /// <summary>
        /// OBB同士の当たり判定
        /// </summary>
        /// <param name="_obb1"> OBB1</param>
        /// <param name="_obb2"> OBB2</param>
        /// <returns> 当たっているか</returns>
        bool IsCollision(const OBB* _obb1, const OBB* _obb2);

        /// <summary>
        /// OBBとSphereの当たり判定
        /// </summary>
        /// <param name="_obb"> OBB</param>
        /// <param name="_sphere"> Sphere</param>
        /// <returns> 当たっているか</returns>
        bool IsCollision(const OBB& _obb, const Sphere& _sphere);

        /// <summary>
        /// OBBを軸に投影
        /// </summary>
        /// <param name="_obb"> OBB</param>
        /// <param name="axis"> 投影する軸</param>
        /// <returns> 投影後の長さ</returns>
        float ProjectOntoAxis(const OBB* _obb, const Vector3& axis);

        /// <summary>
        /// 軸上での重なり判定
        /// </summary>
        /// <param name="_obb1"> OBB1</param>
        /// <param name="_obb2"> OBB2</param>
        /// <param name="axis"> 判定する軸</param>
        /// <returns> 重なっているか</returns>
        bool OverlapOnAxis(const OBB* _obb1, const OBB* _obb2, const Vector3& axis);

    private:

        std::vector<Collider*> colliders_;
        std::vector<std::pair<std::string, std::string>> collisionNames_;
        std::vector<std::pair<std::string, uint32_t>> attributeList_;
        std::list<std::pair<std::string, uint32_t>> maskList_;

        uint32_t countCheckCollision_ = 0ui32;
        uint32_t countWithoutFilter_ = 0ui32;
        uint32_t countWithoutLighter = 0ui32;

    };
}