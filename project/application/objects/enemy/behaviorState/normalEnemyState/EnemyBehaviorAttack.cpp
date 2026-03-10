#include "EnemyBehaviorAttack.h"

#include <Ease.h>

#include "../../NormalEnemy.h"
#include "EnemyBehaviorMove.h"
#include "EnemyBehaviorHitReact.h"
#include "EnemyBehaviorDead.h"

EnemyBehaviorAttack::EnemyBehaviorAttack(NormalEnemy* _pNormalEnemy) : EnemyBehaviorState("Attack", _pNormalEnemy)
{
	// モーションの初期化
	motion_.count = 0;
	motion_.maxCount = attackMotionFrames_; // 攻撃モーションのカウントを設定
   
}

void EnemyBehaviorAttack::Initialize()
{
    attackCooldown_ = initialCooldownFrames_;
}

void EnemyBehaviorAttack::Update()
{
	// 敵のトランスフォームを motion_.transformにセット
	TransformUpdate(pNormalEnemy_);

    // 溜め
    if (motion_.isActive)
    {
        // モーションカウントを更新
        MotionCount(motion_);

        if (motion_.count <= chargeFrames_)
        {
            float t = float(motion_.count) / chargeFrames_; // 0〜1
            float ease = (t <= 0.5f)
                ? Ease::OutSine(t * 2.0f)               // 0〜0.5  → 縮小
                : Ease::OutSine((1.0f - t) * 2.0f);     // 0.5〜1  → 元に戻す

            float scaleValue = 1.0f - ease * chargeScaleShrink_;
            motion_.transform.scale = Vector3(scaleValue, scaleValue, scaleValue);
			pNormalEnemy_->SetObjectScale(motion_.transform.scale);

            float shake = ((motion_.count % chargeShakePeriod_ == 0) ? 1 : -1) * chargeShakeAmp_;
			pNormalEnemy_->SetObjectPosition(motion_.transform.position + Vector3(shake, 0, shake));
            
        }
        // 攻撃
        else
        {
            // 攻撃フラグをリセット
            isAttack_ = false;

            // 回転しながら弾を飛ばす
            if (motion_.count == attackTriggerFrame_)
            {
                // 攻撃フラグを立てる
                isAttack_ = true;
            }

            // Y軸回転
            motion_.transform.rotation.y += attackYawStep_;
			pNormalEnemy_->SetRotation(motion_.transform.rotation);

        }
    }

	if (pNormalEnemy_->GetHP() <= 0)
	{
		// HPが0以下なら、死亡モーションに切り替え
		pNormalEnemy_->ChangeBehaviorState(std::make_unique<EnemyBehaviorDead>(pNormalEnemy_));
        return;
	} 
    else if (pNormalEnemy_->IsHit())
	{
		// ヒットフラグをリセット
		pNormalEnemy_->SetIsHit(false);
        // 無敵化
        pNormalEnemy_->SetIsInvincible(true);
		// ヒットしたら、ヒットリアクションモーションに切り替え
		pNormalEnemy_->ChangeBehaviorState(std::make_unique<EnemyBehaviorHitReact>(pNormalEnemy_));
		return;
	}
    else if (!pNormalEnemy_->IsFarFromPlayer())
	{
		// プレイヤーとの距離が一定以上の場合、移動モーションに切り替え
		pNormalEnemy_->ChangeBehaviorState(std::make_unique<EnemyBehaviorMove>(pNormalEnemy_));
        return;
	}
    else if(!motion_.isActive && motion_.count >= motion_.maxCount)
    {
        // ステートが切り替わらなかったらもう一度
		ResetMotion();

    }

    // クールダウンを減少
    attackCooldown_--;

    // 一定間隔で弾を発射
    if (attackCooldown_ <= 0)
    {
        // 攻撃モーションを開始
        motion_.isActive = true;
        attackCooldown_ = cooldownFrames_;
    }

	// フラグが立っている場合、攻撃を実行
    if (isAttack_)
    {
        pNormalEnemy_->Attack();
    }

}

void EnemyBehaviorAttack::ResetMotion()
{
    // モーションの初期化
    motion_.count = 0;
    motion_.maxCount = attackMotionFrames_; // 攻撃モーションのカウントを設定
}
