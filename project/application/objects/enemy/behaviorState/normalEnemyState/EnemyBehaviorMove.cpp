#include "EnemyBehaviorMove.h"

#include <Ease.h>

#include "../../NormalEnemy.h"
#include "EnemyBehaviorAttack.h"
#include "EnemyBehaviorHitReact.h"

EnemyBehaviorMove::EnemyBehaviorMove(NormalEnemy* _pNormalEnemy) : EnemyBehaviorState("Move", _pNormalEnemy)
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = moveMotionFrames_;
}

void EnemyBehaviorMove::Initialize()
{
}

void EnemyBehaviorMove::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pNormalEnemy_);

	// 動く前に切り替え処理
	if (pNormalEnemy_->IsHit())
	{
		// ヒットフラグをリセット
		pNormalEnemy_->SetIsHit(false);

		// 無敵化
		pNormalEnemy_->SetIsInvincible(true);

		// ヒットしたら、ヒットリアクションモーションに切り替え
		pNormalEnemy_->ChangeBehaviorState(std::make_unique<EnemyBehaviorHitReact>(pNormalEnemy_));
		return;
	}
	else if (pNormalEnemy_->IsFarFromPlayer() && !pNormalEnemy_->IsHitVignetteTrap())
	{
		// プレイヤーとの距離が一定以下の場合、攻撃モーションに切り替え
		pNormalEnemy_->ChangeBehaviorState(std::make_unique<EnemyBehaviorAttack>(pNormalEnemy_));
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
	pNormalEnemy_->SetScale(motion_.transform.scale);

	// モーションカウントを更新
	MotionCount(motion_);

	// 移動
	if (!pNormalEnemy_->IsHitVignetteTrap())
	{
		pNormalEnemy_->Move();
	}
}

void EnemyBehaviorMove::ResetMotion()
{
	motion_.isActive = true;
	motion_.count = 0;
	motion_.maxCount = moveMotionFrames_; // 移動モーションのカウントを設定
}
