#pragma once

#include "../../baseObject/StaticObject.h"
#include "../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <memory>
#include <Framework.h>

/// <summary>
/// バリア
/// プレイヤーを守るバリアオブジェクト
/// </summary>
class Barrier : public StaticObject
{
public:

	Barrier() = default;
	~Barrier() = default;

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;


public: // セッター

	/// <summary>
	/// バリア破壊フラグをセット
	/// </summary>
	/// <param name="_isDestroyed"> 破壊フラグ</param>
	void SetBarrierDestroyed(bool _isDestroyed) { isBarrierDestroyed_ = _isDestroyed; }

private: // 衝突判定

	/// <summary>
	/// 当たり判定処理
	/// </summary>
	/// <param name="_other"> 衝突相手のコライダー</param>
	void OnCollisionTrigger(const Collider* _other);

private:

	// バリア破壊フラグ
	bool isBarrierDestroyed_ = false;

	// 元のスケール
	Vector3 defaultScale_ = { 1.5f, 1.5f, 1.5f };
	// 目標スケール
	Vector3 targetScale_ = { 1.5f, 1.5f, 1.5f };  
	float scaleLerpSpeed_ = 0.2f;

	// 爆発演出用カウント
	uint32_t explodeCount_ = 0;      
	// 爆発演出の継続フレーム数
	const uint32_t explodeMaxCount_ = 10; 
	bool isExploding_ = false;

};