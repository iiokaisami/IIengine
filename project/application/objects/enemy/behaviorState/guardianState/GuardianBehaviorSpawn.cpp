#include "GuardianBehaviorSpawn.h"

#include <Ease.h>

#include "../../Guardian.h"
#include "GuardianBehaviorMove.h"

GuardianBehaviorSpawn::GuardianBehaviorSpawn(Guardian* _pGuardian) : GuardianBehaviorState("Spawn", _pGuardian)
{
	// モーションの初期化
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = spawnCount_; // スポーンモーションのカウントを設定
}

void GuardianBehaviorSpawn::Initialize()
{

}

void GuardianBehaviorSpawn::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pGuardian_);

	// イージングの進行度(0〜1)
	float t = float(motion_.count) / motion_.maxCount;
	motion_.transform.scale.x = Ease::OutBack(t);
	motion_.transform.scale.y = Ease::OutBack(t);
	motion_.transform.scale.z = Ease::OutBack(t);
	Vector3 one = { 1.0f, 1.0f, 1.0f };

	pGuardian_->SetObjectPosition(motion_.transform.position);
	pGuardian_->SetObjectRotation(motion_.transform.rotation);
	pGuardian_->SetObjectScale(one * motion_.transform.scale);

	// モーションカウントを更新
	MotionCount(motion_);

	if (!motion_.isActive)
	{
		// 無敵状態を解除
		pGuardian_->SetInvincible(false);

		// スポーンモーションが終了したら、次の状態に移行
		pGuardian_->ChangeBehaviorState(std::make_unique<GuardianBehaviorMove>(pGuardian_));
	}


}

void GuardianBehaviorSpawn::ResetMotion()
{
}
