#pragma once

#include "../../baseObject/StaticObject.h"
#include "../../../gameEngine/collider/ColliderManager.h"
#include "../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <memory>
#include <Framework.h>
#include <random>

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

public: // ゲッター

	// バリア破壊完了フラグを取得
	bool IsBarrierDestroyedComplete() const { return isBarrierDestroyedComplete_; }

private: // 衝突判定

	/// <summary>
	/// 当たり判定処理
	/// </summary>
	/// <param name="_other"> 衝突相手のコライダー</param>
	void OnCollisionTrigger(const IIEngine::Collider* _other);

private:

	/// <summary>
	/// シェイク更新
	/// </summary>
	/// <param name="deltaTime">デルタタイム</param>
	void UpdateShake();

private:

	// バリア破壊フラグ
	bool isBarrierDestroyed_ = false;
	// バリア破壊完了フラグ
	bool isBarrierDestroyedComplete_ = false;

	// 元のスケール
	Vector3 defaultScale_ = { 1.5f, 1.5f, 1.5f };
	// 目標スケール
	Vector3 targetScale_ = { 1.5f, 1.5f, 1.5f };  

	// 爆発演出用カウント
	float explodeCount_ = 0.0f;      
	// 爆発演出の継続
	const float explodeMaxCount_ = 0.1f; 
	bool isExploding_ = false;

	float hitScaleMultiplier_ = 1.5f;
	
	// シェイクパラメーター
	float shakeDuration_ = 0.0f;
	float shakeMagnitude_ = 0.0f;
	float shakeTimeElapsed_ = 0.0f;
	std::default_random_engine randomEngine_;
	std::uniform_real_distribution<float> randomDistribution_;
	const float shakeMaxCount_ = 1.5f;
    Vector3 basePosition_;

	// 破裂パラメーター
	float explodeScalePeak_ = 2.0f;
	uint32_t explodeParticleCount_ = 8;
	
	// スパークパラメーター
	float sparkEmitHeight_ = 3.0f;
	uint32_t sparkParticleCount_ = 1;
	float sparkMinVisibleScaleX_ = 0.1f;
	float scaleReturnEpsilon_ = 0.01f;

};