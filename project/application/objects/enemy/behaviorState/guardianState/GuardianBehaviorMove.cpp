#include "GuardianBehaviorMove.h"

#include "../../Guardian.h"
#include "GuardianBehaviorAttack.h"
#include "GuardianBehaviorHitReact.h"

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
	//else if (pGuardian_->IsFarFromPlayer())
	//{
	//	// プレイヤーとの距離が一定以下の場合、攻撃モーションに切り替え
	//	pGuardian_->ChangeBehaviorState(std::make_unique<GuardianBehaviorAttack>(pGuardian_));
	//	return;
	//}
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
