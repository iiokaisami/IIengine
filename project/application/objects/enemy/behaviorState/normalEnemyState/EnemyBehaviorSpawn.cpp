#include "EnemyBehaviorSpawn.h"

#include <Ease.h>

#include "../../NormalEnemy.h"
#include "EnemyBehaviorMove.h"

EnemyBehaviorSpawn::EnemyBehaviorSpawn(NormalEnemy* _pNormalEnemy) : EnemyBehaviorState("Spawn", _pNormalEnemy)
{
	// モーションの初期化
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = 30; // スポーンモーションのカウントを設定
}

void EnemyBehaviorSpawn::Initialize()
{
}

void EnemyBehaviorSpawn::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pNormalEnemy_);

	// イージングの進行度（0〜1）
	float t = float(motion_.count) / motion_.maxCount;
	motion_.transform.scale.x = Ease::OutBack(t);
	motion_.transform.scale.y = Ease::OutBack(t);
	motion_.transform.scale.z = Ease::OutBack(t);
	Vector3 one(1.0f, 1.0f, 1.0f);

	pNormalEnemy_->SetObjectPosition(motion_.transform.position);
	pNormalEnemy_->SetObjectRotation(motion_.transform.rotation);
	pNormalEnemy_->SetObjectScale(one * motion_.transform.scale);

	// モーションカウントを更新
	MotionCount(motion_);

	if (!motion_.isActive)
	{
		// 無敵状態を解除
		pNormalEnemy_->SetIsInvincible(false);

		// スポーンモーションが終了したら、次の状態に移行
		pNormalEnemy_->ChangeBehaviorState(std::make_unique<EnemyBehaviorMove>(pNormalEnemy_));
	}
}

void EnemyBehaviorSpawn::ResetMotion()
{
}
