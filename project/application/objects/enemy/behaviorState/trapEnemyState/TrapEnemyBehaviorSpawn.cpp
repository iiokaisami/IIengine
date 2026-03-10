#include "TrapEnemyBehaviorSpawn.h"

#include <Ease.h>

#include "../../TrapEnemy.h"
#include "TrapEnemyBehaviorMove.h"

TrapEnemyBehaviorSpawn::TrapEnemyBehaviorSpawn(TrapEnemy* _pTrapEnemy) : TrapEnemyBehaviorState("Spawn", _pTrapEnemy)
{
	// モーションの初期化
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = spawnMotionFrames_; // スポーンモーションのカウントを設定
}

void TrapEnemyBehaviorSpawn::Initialize()
{
}

void TrapEnemyBehaviorSpawn::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pTrapEnemy_);

	// イージングの進行度
	float t = float(motion_.count) / motion_.maxCount;
	motion_.transform.scale.x = Ease::OutBack(t);
	motion_.transform.scale.y = Ease::OutBack(t);
	motion_.transform.scale.z = Ease::OutBack(t);
	Vector3 one(1.0f, 1.0f, 1.0f);

	pTrapEnemy_->SetObjectPosition(motion_.transform.position);
	pTrapEnemy_->SetObjectRotation(motion_.transform.rotation);
	pTrapEnemy_->SetObjectScale(one * motion_.transform.scale);

	// モーションカウントを更新
	MotionCount(motion_);

	if (!motion_.isActive)
	{
		// 無敵状態を解除
		pTrapEnemy_->SetIsInvincible(false);

		// スポーンモーションが終了したら、次の状態に移行
		pTrapEnemy_->ChangeBehaviorState(std::make_unique<TrapEnemyBehaviorMove>(pTrapEnemy_));
	}
}

void TrapEnemyBehaviorSpawn::ResetMotion()
{
}
