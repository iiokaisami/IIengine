#pragma once

#include "GuardianBehaviorState.h"

class GuardianBehaviorDead : public GuardianBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pGuardian">ガーディアンポインタ</param>
	GuardianBehaviorDead(Guardian* _pGuardian);
	
	// デストラクタ
	~GuardianBehaviorDead() = default;
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのカウント等をリセット
	void ResetMotion() override;

private:

	// モーションのカウント
	uint32_t deadCount_ = 30;

	// 初回膨張
	float startPopScale_ = 2.8f;
	// 最終縮小
	float endScale_ = 0.0f;
	// 上昇量
	float floatUpAmount_ = 0.2f;
	// 回転速度
	float rotSpeedY_ = 0.1f;
	float rotSpeedX_ = 0.1f;

	// パーティクルの数
	int petalParticleCount_ = 6;
	int ruptureParticleCount_ = 20;

};