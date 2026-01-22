#include "TrapEnemyBehaviorState.h"

#include "../../TrapEnemy.h"

TrapEnemyBehaviorState::~TrapEnemyBehaviorState()
{
}

void TrapEnemyBehaviorState::MotionCount(Motion& _motion)
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

void TrapEnemyBehaviorState::TransformUpdate(TrapEnemy* _pEnemy)
{
	if (_pEnemy)
	{
		motion_.transform.position = _pEnemy->GetPosition();
		motion_.transform.rotation = _pEnemy->GetRotation();
		motion_.transform.scale = _pEnemy->GetScale();
	}
}
