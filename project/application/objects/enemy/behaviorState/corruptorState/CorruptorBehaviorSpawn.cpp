#include "CorruptorBehaviorSpawn.h"

#include <Ease.h>

#include "../../Corruptor.h"
#include "CorruptorBehaviorMove.h"

CorruptorBehaviorSpawn::CorruptorBehaviorSpawn(Corruptor* _pCorruptor) : CorruptorBehaviorState("Spawn", _pCorruptor)
{
	// モーションの初期化
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = 30; // スポーンモーションのカウントを設定
}

void CorruptorBehaviorSpawn::Initialize()
{
}

void CorruptorBehaviorSpawn::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pCorruptor_);

	// イージングの進行度
	float t = float(motion_.count) / motion_.maxCount;

	// 回転
	motion_.transform.rotation.y = Lerp(0.0f, std::numbers::pi_v<float> *2.0f, Ease::OutCubic(t));

	// トランスフォームを敵に反映
	pCorruptor_->SetObjectPosition(motion_.transform.position);

	// モーションカウントを更新
	MotionCount(motion_);

	if (!motion_.isActive)
	{
		// 無敵状態を解除
		pCorruptor_->SetIsInvincible(false);

		// スポーンモーションが終了したら、次の状態に移行
		pCorruptor_->ChangeBehaviorState(std::make_unique<CorruptorBehaviorMove>(pCorruptor_));
	}
}

void CorruptorBehaviorSpawn::ResetMotion()
{
}
