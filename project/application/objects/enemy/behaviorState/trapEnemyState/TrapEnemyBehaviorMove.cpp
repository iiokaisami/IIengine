#include "TrapEnemyBehaviorMove.h"

#include <Ease.h>

#include "../../TrapEnemy.h"
#include "TrapEnemyBehaviorSetTrap.h"
#include "TrapEnemyBehaviorHitReact.h"

TrapEnemyBehaviorMove::TrapEnemyBehaviorMove(TrapEnemy* _pTrapEnemy) : TrapEnemyBehaviorState("Move", _pTrapEnemy)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = moveMotionFrames_; // 移動モーションのカウントを設定
}

void TrapEnemyBehaviorMove::Initialize()
{
	trapCooldown_ = kMaxTrapCooldown;
}

void TrapEnemyBehaviorMove::Update()
{

	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pTrapEnemy_);

	// 0になったら元に戻す
	if (trapCooldown_ == 0)
	{
		trapCooldown_ = kMaxTrapCooldown;
	}

	// クールタイム進行
	trapCooldown_--;

	// クールタイムが完了しているかどうかをチェック
	pTrapEnemy_->SetIsTrapCooldownComplete((trapCooldown_ <= 0));


	// 動く前に切り替え処理
	if (pTrapEnemy_->IsHit())
	{
		// ヒットフラグをリセット
		pTrapEnemy_->SetIsHit(false);

		// 無敵化
		pTrapEnemy_->SetIsInvincible(true);

		// ヒットしたら、ヒットリアクションモーションに切り替え
		pTrapEnemy_->ChangeBehaviorState(std::make_unique<TrapEnemyBehaviorHitReact>(pTrapEnemy_));
		return;
	}
	else if (pTrapEnemy_->IsStopAndTrap() && pTrapEnemy_->IsTrapCooldownComplete())
	{
		// プレイヤーとの距離が一定以下の場合、攻撃モーションに切り替え
		pTrapEnemy_->ChangeBehaviorState(std::make_unique<TrapEnemyBehaviorSetTrap>(pTrapEnemy_));
		return;
	} 
	else
	{
		// ステートが切り替わらなかったらもう一度
		ResetMotion();
	}

	// イージング進行度（0〜1）
	float t = float(motion_.count) / motion_.maxCount;

	// ポヨポヨ拡大縮小（X・Y両方）
	float scaleWaveX = 1.0f + std::sin(t * std::numbers::pi_v<float>) * moveScaleWaveAmp_;
	float scaleWaveY = 1.0f + std::cos(t * std::numbers::pi_v<float>) * moveScaleWaveAmp_;

	// スケールを更新（X・Y両方ポヨポヨ）
	motion_.transform.scale = Vector3(scaleWaveX, scaleWaveY, moveScaleZ_);

	// スケールをセット
	pTrapEnemy_->SetScale(motion_.transform.scale);

	// モーションカウントを更新
	MotionCount(motion_);

	// 移動
	pTrapEnemy_->Move();

}

void TrapEnemyBehaviorMove::ResetMotion()
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = moveMotionFrames_; // 移動モーションのカウントを設定
}
