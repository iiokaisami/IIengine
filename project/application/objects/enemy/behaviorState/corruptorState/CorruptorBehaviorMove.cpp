#include "CorruptorBehaviorMove.h"

#include <Ease.h>

#include "../../Corruptor.h"
#include "CorruptorBehaviorSelfDestruct.h"

CorruptorBehaviorMove::CorruptorBehaviorMove(Corruptor* _pCorruptor) : CorruptorBehaviorState("Move", _pCorruptor)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = 30; // 移動モーションのカウントを設定
}

void CorruptorBehaviorMove::Initialize()
{
}

void CorruptorBehaviorMove::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pCorruptor_);
	
	// 動く前に切り替え処理
	if (pCorruptor_->IsHit() or pCorruptor_->IsFarFromPlayer())
	{
		// ヒットフラグをリセット
		pCorruptor_->SetIsHit(false);

		// 無敵化
		pCorruptor_->SetIsInvincible(true);

		// ヒットしたら、自爆モーションに切り替え
		pCorruptor_->ChangeBehaviorState(std::make_unique<CorruptorBehaviorSelfDestruct>(pCorruptor_));
		
		return;
	}
	else if (!motion_.isActive)
	{
		// ステートが切り替わらなかったらもう一度
		ResetMotion();
	}
	
	// イージング進行度（0〜1）
	float t = float(motion_.count) / motion_.maxCount;

	// ポヨポヨ拡大縮小（X・Y両方）
	float scaleWaveX = 1.0f + std::sin(t * std::numbers::pi_v<float>) * 0.2f;
	float scaleWaveY = 1.0f + std::cos(t * std::numbers::pi_v<float>) * 0.2f;

	// スケールを更新（X・Y両方ポヨポヨ）
	motion_.transform.scale = Vector3(scaleWaveX, scaleWaveY, 1.0f);

	// スケールを適用
	pCorruptor_->SetObjectScale(motion_.transform.scale);

	// モーションカウントを更新
	MotionCount(motion_);

	// 移動
	pCorruptor_->Move();

}

void CorruptorBehaviorMove::ResetMotion()
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = 30; // 移動モーションのカウントを設定
}
