#include "GuardianBehaviorState.h"

#include "../../Guardian.h"

GuardianBehaviorState::~GuardianBehaviorState()
{
}

void GuardianBehaviorState::MotionCount(Motion& _motion)
{
	if (_motion.count < _motion.maxCount)
	{
		_motion.count++;
	}
	else
	{
		// モーション終了 カウントリセット
		_motion.isActive = false;
		_motion.count = 0;
	}
}

void GuardianBehaviorState::TransformUpdate(Guardian* _pGuardian)
{
	if (_pGuardian)
	{
		motion_.transform.position = _pGuardian->GetPosition();
		motion_.transform.rotation = _pGuardian->GetRotation();
		motion_.transform.scale = _pGuardian->GetScale();
	}
}