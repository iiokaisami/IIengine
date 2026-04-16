#pragma once

#include "../../baseObject/StaticObject.h"
#include "../../../gameEngine/collider/ColliderManager.h"
#include "Barrier.h"

#include <Object3d.h>
#include <memory>
#include <Framework.h>

/// <summary>
/// ゴール
/// </summary>
class Goal : public StaticObject
{
public:

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

public: // セッター

	/// <summary>
	/// ゴール出現フラグをセット
	/// </summary>
	/// <param name="_isCleared"> クリアフラグ</param>
	void SetBarrierDestroyed(bool _isDestroyed) { isBarrierDestroyed_ = _isDestroyed; }
	
public: // ゲッター

	// クリアフラグを取得
	bool IsCleared() const { return isCleared_; }

	// バリア破壊フラグを取得
	bool IsBarrierDestroyed() const { return pBarrier_->IsBarrierDestroyedComplete(); }

private: // 衝突判定

	/// <summary>
	/// 当たり判定処理
	/// </summary>
	/// <param name="_other"> 衝突相手のコライダー</param>
	void OnCollisionTrigger(const IIEngine::Collider* _other);

private:

	// バリア
	std::unique_ptr<Barrier> pBarrier_ = nullptr;

	// クリアフラグ
	bool isCleared_ = false;

	// バリア破壊フラグ
	bool isBarrierDestroyed_ = false;
	bool wasBarrierDestroyed_ = false;

	// デフォルト値
	Vector3 goalInitialPos_ = { 0.0f, 0.0f, -3.5f };
	Vector3 goalInitialScale_ = { 1.0f, 1.0f, 1.0f };

	// パーティクル数
	int clearedParticleCount_ = 2;

	// 演出パラメータ
	float tiltAngleRad_ = 0.35f;            // 斜め
	float rotateSpeedRadPerSec_ = 1.5f;     // y回転速度
	float bobAmplitude_ = 0.3f;            // 上下の振れ幅
	float bobFreqRadPerSec_ = 2.0f;         // 上下の角周波数
	float bobTimeSec_ = 0.0f;               // 経過時間

};