#include "EnemyBehaviorDead.h"

#include <Ease.h>

#include "../../NormalEnemy.h"

EnemyBehaviorDead::EnemyBehaviorDead(NormalEnemy* _pNormalEnemy) : EnemyBehaviorState("Dead", _pNormalEnemy)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = deadFrames_; // 移動モーションのカウントを設定
}

void EnemyBehaviorDead::Initialize()
{
}

void EnemyBehaviorDead::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pNormalEnemy_);

	float t = float(motion_.count) / motion_.maxCount;

	if (motion_.count == 0)
	{
		motion_.transform.scale = Vector3(startPopScale_, startPopScale_, startPopScale_); // 初回だけ一気に膨らむ
	}

	// 徐々に縮む演出(1.8 → 0.0)
	float scale = Lerp(startPopScale_, endScale_, Ease::InCubic(t));
	motion_.transform.scale = (Vector3(scale, scale, scale));

	motion_.transform.position.y += Ease::OutQuad(t) * floatUpAmount_;

	motion_.transform.rotation.y += rotSpeedY_;
	motion_.transform.rotation.x += rotSpeedX_;

	pNormalEnemy_->SetObjectPosition(motion_.transform.position);
	pNormalEnemy_->SetObjectRotation(motion_.transform.rotation);
	pNormalEnemy_->SetObjectScale(motion_.transform.scale);


	// モーションカウントを更新
	MotionCount(motion_);

	if (!motion_.isActive)
	{
		// 死亡モーションが終了したら、敵を削除
		pNormalEnemy_->SetIsDead(true);

		// パーティクル
		IIEngine::ParticleEmitter::Emit("petalGroup", motion_.transform.position, petalParticleCount_);
		IIEngine::ParticleEmitter::Emit("rupture", motion_.transform.position, ruptureParticleCount_);
	}

}

void EnemyBehaviorDead::ResetMotion()
{
}
