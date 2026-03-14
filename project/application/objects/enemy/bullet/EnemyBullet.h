#pragma once

#include "../../../baseObject/BaseBullet.h"
#include"../../../../gameEngine/collider/ColliderManager.h"
#include "../../../../gameEngine/particle/ParticleEmitter.h"

#include <Object3d.h>

/// <summary>
/// 敵の弾クラス
/// </summary>
class EnemyBullet : public BaseBullet
{
public:
	EnemyBullet() = default;
	~EnemyBullet() = default;

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
	/// 速度設定
	/// </summary>
	/// <param name="_velocity">速度ベクトル</param>
	/// <returns>設定後の速度ベクトル</returns>
	Vector3 SetVelocity(const Vector3& _velocity)
	{ 
		velocity_ = _velocity * kDefaultFrameRate;
		return velocity_;
	}

private: // メンバ変数
	
	// 弾の基本サイズ
	float baseScale_ = 0.7f;               
	// 回転速度
	float spinSpeed_ = 0.1f;               
	// 命中時リアクション
	int hitParticleCount_ = 1;             

	// 寿命
	static const uint32_t kLifeTime = 60 * 3;


};

