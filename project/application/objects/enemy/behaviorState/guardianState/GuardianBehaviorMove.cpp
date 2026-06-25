#include "GuardianBehaviorMove.h"

#include "../../Guardian.h"
#include "GuardianBehaviorHitReact.h"
#include "GuardianBehaviorJump.h"

GuardianBehaviorMove::GuardianBehaviorMove(Guardian* _pGuardian) : GuardianBehaviorState("Move", _pGuardian)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = moveCount_;
}

void GuardianBehaviorMove::Initialize()
{
}

void GuardianBehaviorMove::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pGuardian_);

	// 動く前に切り替え処理
	if (pGuardian_->IsHit())
	{
		// ヒットフラグをリセット
		pGuardian_->SetIsHit(false);
	
		// 無敵化
		pGuardian_->SetInvincible(true);
		
		// ヒットしたら、ヒットリアクションモーションに切り替え
		pGuardian_->ChangeBehaviorState(std::make_unique<GuardianBehaviorHitReact>(pGuardian_));
		return;
	}
	else if (pGuardian_->GetJumping())
	{
		// ジャンプ開始したら、ジャンプモーションに切り替え
		pGuardian_->ChangeBehaviorState(std::make_unique<GuardianBehaviorJump>(pGuardian_));
		return;
	}
	else if (pGuardian_->GetHP() <= pGuardian_->GetJumpStartHP() &&
		     pGuardian_->GetAttackCycle() >= pGuardian_->GetJumpCycle())
	{
		// HPがジャンプ開始HP以下になったら、ジャンプモーションに切り替え
		pGuardian_->ChangeBehaviorState(std::make_unique<GuardianBehaviorJump>(pGuardian_));
		return;
	}
	else
	{
		// ステートが切り替わらなかったらもう一度
		ResetMotion();
	}

	pGuardian_->Move();

}

void GuardianBehaviorMove::ResetMotion()
{

}
