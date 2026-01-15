#include "EnemyBehaviorHitReact.h"

#include <Ease.h>

#include "../../NormalEnemy.h"
#include "EnemyBehaviorMove.h"
#include "EnemyBehaviorDead.h"

EnemyBehaviorHitReact::EnemyBehaviorHitReact(NormalEnemy* _pNormalEnemy) : EnemyBehaviorState("Hit", _pNormalEnemy)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = 30; // 移動モーションのカウントを設定
}

void EnemyBehaviorHitReact::Initialize()
{
}

void EnemyBehaviorHitReact::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pNormalEnemy_);

	Vector3 shakeOffset =
	{
	((motion_.count % 2 == 0) ? 1.0f : -1.0f) * 0.1f,
	0.0f, // Y軸は揺らさない
	((motion_.count % 3 == 0) ? 1.0f : -1.0f) * 0.1f
	};

	Vector3 originPos = pNormalEnemy_->GetPosition();

	pNormalEnemy_->SetObjectPosition(originPos + shakeOffset);

	// モーションカウントを更新
	MotionCount(motion_);

	if (!motion_.isActive)
	{
 		if (pNormalEnemy_->GetHP() <= 0)
		{
			// HPが0以下なら、死亡モーションに切り替え
			pNormalEnemy_->ChangeBehaviorState(std::make_unique<EnemyBehaviorDead>(pNormalEnemy_));
			return;
		} 
		else if(pNormalEnemy_->GetHP() > 0)
		{
			float hp = 0;
			hp = pNormalEnemy_->GetHP();

			// 無敵状態を解除
 			pNormalEnemy_->SetIsInvincible(false);

			// ヒットリアクションモーションが終了したら、移動モーションに切り替え
			pNormalEnemy_->ChangeBehaviorState(std::make_unique<EnemyBehaviorMove>(pNormalEnemy_));
		}
	}

}

void EnemyBehaviorHitReact::ResetMotion()
{
}
