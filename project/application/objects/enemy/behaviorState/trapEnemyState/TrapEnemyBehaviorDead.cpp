#include "TrapEnemyBehaviorDead.h"

#include <Ease.h>

#include "../../TrapEnemy.h"

TrapEnemyBehaviorDead::TrapEnemyBehaviorDead(TrapEnemy* _pTrapEnemy) : TrapEnemyBehaviorState("Dead", _pTrapEnemy)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = 30; // モーションのカウントを設定
}

void TrapEnemyBehaviorDead::Initialize()
{
}

void TrapEnemyBehaviorDead::Update()
{
	// 敵のトランスフォームをmotion_.transformにセット
	TransformUpdate(pTrapEnemy_);

	float t = float(motion_.count) / motion_.maxCount;

	if (motion_.count == 0)
	{
		motion_.transform.scale = Vector3(1.8f, 1.8f, 1.8f); // 初回だけ一気に膨らむ
	}

	// 徐々に縮む演出
	float scale = Lerp(1.8f, 0.0f, Ease::InCubic(t));
	motion_.transform.scale = (Vector3(scale, scale, scale));

	motion_.transform.position.y += Ease::OutQuad(t) * 0.1f;

	motion_.transform.rotation.y += 0.1f;
	motion_.transform.rotation.x += 0.1f;

	pTrapEnemy_->SetObjectPosition(motion_.transform.position);
	pTrapEnemy_->SetObjectRotation(motion_.transform.rotation);
	pTrapEnemy_->SetObjectScale(motion_.transform.scale);


	// モーションカウントを更新
	MotionCount(motion_);

	if (!motion_.isActive)
	{
		// 死亡モーションが終了したら、敵を削除
		pTrapEnemy_->SetIsDead(true);

		// パーティクル
		ParticleEmitter::Emit("explosionGroup", motion_.transform.position, 6);
		ParticleEmitter::Emit("rupture", motion_.transform.position, 20);
	}
}

void TrapEnemyBehaviorDead::ResetMotion()
{
}
