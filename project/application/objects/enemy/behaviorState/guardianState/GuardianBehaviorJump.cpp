#include "GuardianBehaviorJump.h"

#include "../../Guardian.h"
#include "GuardianBehaviorMove.h"

GuardianBehaviorJump::GuardianBehaviorJump(Guardian* _pGuardian) : GuardianBehaviorState("Jump", _pGuardian)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = moveCount_;
}

void GuardianBehaviorJump::Initialize()
{
	// ジャンプ開始
	pGuardian_->StartJump();
}

void GuardianBehaviorJump::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pGuardian_);

	// ジャンプ攻撃
	pGuardian_->JumpAttack();

	if (pGuardian_->GetLanding())
	{
		// ジャンプフラグをリセット
		pGuardian_->SetJumping(false);
		// 着地フラグをリセット
		pGuardian_->SetLanding(false);

		// 着地したら、移動モーションに切り替え
		pGuardian_->ChangeBehaviorState(std::make_unique<GuardianBehaviorMove>(pGuardian_));
		
		return;
	}

}

void GuardianBehaviorJump::ResetMotion()
{}
