#include "TrapEnemyBehaviorHitReact.h"

#include <Ease.h>

#include "../../TrapEnemy.h"
#include "TrapEnemyBehaviorMove.h"
#include "TrapEnemyBehaviorDead.h"

TrapEnemyBehaviorHitReact::TrapEnemyBehaviorHitReact(TrapEnemy* _pTrapEnemy) : TrapEnemyBehaviorState("Hit", _pTrapEnemy)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = 30; // 移動モーションのカウントを設定
}

void TrapEnemyBehaviorHitReact::Initialize()
{
}

void TrapEnemyBehaviorHitReact::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pTrapEnemy_);

	Vector3 shakeOffset =
	{
	((motion_.count % 2 == 0) ? 1.0f : -1.0f) * 0.1f,
	0.0f, // Y軸は揺らさない
	((motion_.count % 3 == 0) ? 1.0f : -1.0f) * 0.1f
	};

	Vector3 originPos = pTrapEnemy_->GetPosition();

	pTrapEnemy_->SetObjectPosition(originPos + shakeOffset);

	// モーションカウントを更新
	MotionCount(motion_);

	if (!motion_.isActive)
	{
		if (pTrapEnemy_->GetHP() <= 0)
		{
			// HPが0以下なら、死亡モーションに切り替え
			pTrapEnemy_->ChangeBehaviorState(std::make_unique<TrapEnemyBehaviorDead>(pTrapEnemy_));
			return;
		}
		else
		{
			// 無敵状態を解除
			pTrapEnemy_->SetIsInvincible(false);

			// ヒットリアクションモーションが終了したら、移動モーションに切り替え
			pTrapEnemy_->ChangeBehaviorState(std::make_unique<TrapEnemyBehaviorMove>(pTrapEnemy_));
		}
	}

}

void TrapEnemyBehaviorHitReact::ResetMotion()
{
}
