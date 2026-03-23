#pragma once

#include "../../../baseObject/BaseBullet.h"
#include "../../../../gameEngine/collider/ColliderManager.h"
#include "../../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>
#include <Sprite.h>

/// <summary>
/// プレイヤー弾クラス
/// </summary>
class PlayerBullet : public BaseBullet
{
public:

	PlayerBullet() = default;
	~PlayerBullet() = default;
	
	// 初期化
	void Initialize() override;
	
	// 終了
	void Finalize() override;
	
	// 更新
	void Update() override;
	
	// 描画
	void Draw() override;
	
	// スプライト描画
	void Draw2D();
	
	// ImGui
	void ImGuiDraw();
	
private: // 衝突判定

	/// <summary>
	/// 衝突時処理
	/// </summary>
	/// <param name="_other">衝突相手のコライダー</param>
	void OnCollisionTrigger(const IIEngine::Collider* _other);

public: // セッター

	/// <summary>
	/// 速度取得
	/// </summary>
	/// <returns>速度ベクトル</returns>
	void SetVelocity(const Vector3& _velocity) override
	{
		velocity_ = _velocity * kDefaultFrameRate; 
	}

private: // メンバ変数

	// 寿命
	static const int32_t kLifeTime = 60 * 5;

	// スケール
	Vector3 bulletScale_ = { 0.5f, 0.5f, 0.5f };
	// スピン速度
	float spinSpeed_ = 1.0f;
	// スラッシュパーティクルの発生数
	int slashParticleCount_ = 1;
	// 寿命時間
	float lifeTimeSec_ = 5.0f;

};