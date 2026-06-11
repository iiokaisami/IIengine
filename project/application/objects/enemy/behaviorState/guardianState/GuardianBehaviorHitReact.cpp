#include "GuardianBehaviorHitReact.h"

#include "../../Guardian.h"
#include "GuardianBehaviorMove.h"
#include "GuardianBehaviorDead.h"

GuardianBehaviorHitReact::GuardianBehaviorHitReact(Guardian* _pGuardian) : GuardianBehaviorState("HitReact", _pGuardian)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = hitReactCount_; // 移動モーションのカウントを設定
}

void GuardianBehaviorHitReact::Initialize()
{
}

void GuardianBehaviorHitReact::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
   	TransformUpdate(pGuardian_);
	
	Vector3 shakeOffset =
	{
	((motion_.count % shakePeriodX_ == 0) ? 2.0f : -2.0f) * shakeAmp_,
	0.0f, // Y軸は揺らさない
	((motion_.count % shakePeriodZ_ == 0) ? 2.0f : -2.0f) * shakeAmp_
	};
	
	Vector3 originPos = pGuardian_->GetPosition();
	
	pGuardian_->SetObjectPosition(originPos + shakeOffset);
	
	// モーションカウントを更新
	MotionCount(motion_);
	
	if (!motion_.isActive)
	{
		if (pGuardian_->GetHP() <= 0)
		{
			// HPが0以下なら、死亡モーションに切り替え
			pGuardian_->ChangeBehaviorState(std::make_unique<GuardianBehaviorDead>(pGuardian_));
			return;
		} 
		else if(pGuardian_->GetHP() > 0)
		{
			float hp = 0;
			hp = pGuardian_->GetHP();
			// 無敵状態を解除
			pGuardian_->SetInvincible(false);
			// ヒットリアクションモーションが終了したら、移動モーションに切り替え
			pGuardian_->ChangeBehaviorState(std::make_unique<GuardianBehaviorMove>(pGuardian_));
		}
	}

}

void GuardianBehaviorHitReact::ResetMotion()
{
}
