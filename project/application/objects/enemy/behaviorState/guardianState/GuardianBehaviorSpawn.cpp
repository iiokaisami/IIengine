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
