#include "CorruptorBehaviorState.h"

#include "../../Corruptor.h"

CorruptorBehaviorState::~CorruptorBehaviorState()
{
}

void CorruptorBehaviorState::MotionCount(Motion& _motion)
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

void CorruptorBehaviorState::TransformUpdate(Corruptor* _pEnemy)
{
	if (_pEnemy)
	{
		motion_.transform.position = _pEnemy->GetPosition();
		motion_.transform.rotation = _pEnemy->GetRotation();
		motion_.transform.scale = _pEnemy->GetScale();
	}
}
