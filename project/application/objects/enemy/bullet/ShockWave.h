#pragma once

#include "../../../baseObject/BaseBullet.h"
#include"../../../../gameEngine/collider/ColliderManager.h"

#include <Object3d.h>

/// <summary>
/// 衝撃波クラス
/// </summary>
class ShockWave : public BaseBullet
{
public:

	ShockWave() = default;
	~ShockWave() = default;

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

private: // 衝突判定

	/// <summary>
	/// 衝突時処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollisionTrigger(const IIEngine::Collider* _other);

public: // ゲッター

public: // セッター

private: // メンバ変数

	// 基本サイズ
	float baseScale_ = 0.5f;

	// 拡大速度
	float expansionSpeed_ = 0.2f;

	// 寿命
	static const uint32_t kLifeTime = 60 * 5;

	// 消滅開始
	bool isFading_ = false;

	// 衝突済みフラグ
	bool hasCollided_ = false;

	// シェイクの振幅
	float shakeAmplitude_ = 0.05f;
	// シェイクの周波数
	float shakeFrequency_ = 10.0f;

};
