#include "FollowController.h"

#include <Camera.h>

FollowController::FollowController(std::shared_ptr<IIEngine::Camera> camera, std::function<Vector3()> targetPos, std::function<Vector3()> targetVelocity, float positionLerp, float rotationLerp, Vector3 targetRot)
	: camera_(camera),
      targetPos_(targetPos),
	  targetVelocity_(targetVelocity),
      positionLerp_(positionLerp),
	  rotationLerp_(rotationLerp),
	  targetRot_(targetRot),
      smoothedVelocity_({ 0.0f, 0.0f, 0.0f }),
      blendingProgress_(0.0f)
{}

void FollowController::Update(float dt)
{
    // 無効またはカメラがない場合は処理しない
    if (!active_ or !camera_)
    {
        return;
    }

    // プレイヤーの現在位置と速度を取得
    Vector3 playerPos = targetPos_();
    Vector3 playerVelocity = targetVelocity_();

    // 現在のカメラ位置を取得
    Vector3 currentPos = camera_->GetPosition();

    // プレイヤーが現在動いているか
    bool isPlayerMoving = playerVelocity.Length() > 0.1f;

    if (isPlayerMoving)
    {
        // 静止タイマーをリセットし、カメラ追尾遅延タイマーを増加
        stopTimer_ = 0.0f;

        if (!isFollowing_)
        {
            followTimer_ += dt;
            if (followTimer_ >= followDelay_)
            {
                isFollowing_ = true;
                blendingProgress_ = 0.0f; // 初期化
            }
        }
    } 
    else // プレイヤーが静止している場合
    {
        followTimer_ = 0.0f;
        if (isFollowing_)
        {
            stopTimer_ += dt;
            if (stopTimer_ >= stopDelay_) // カメラ停止遅延判定
            {
                isFollowing_ = false; // 追尾終了
                lastStopPosition_ = playerPos; // 最後のプレイヤーの位置を記録する
                blendingProgress_ = 0.0f; // Blendの初期化
            }
        }
    }

    // カメラ目標位置を設定
    Vector3 targetPos;
    if (isFollowing_)
    {
        if (blendingProgress_ < 1.0f)
        {
            // 追尾再開時徐々にカメラを移行
            blendingProgress_ = std::min(blendingProgress_ + (dt / followDelay_), 1.0f);
            Vector3 temp;
            temp.Lerp(lastStopPosition_ + offset_, playerPos + offset_ - smoothedVelocity_, blendingProgress_);
            targetPos = temp;
        } 
        else
        {
            // 通常の追尾
            targetPos = playerPos + offset_ - smoothedVelocity_;
        }
    }
    else
    {
        // 停止時もカメラの位置をなめらかに補間してズレを解消
        blendingProgress_ = std::min(blendingProgress_ + dt / stopDelay_, 1.0f); // 停止補間の進行度
        Vector3 temp;
        temp.Lerp(camera_->GetPosition(), lastStopPosition_ + offset_, blendingProgress_); // 停止時のスムーズな追従
        targetPos = temp;
    }

    // スムーズに現在位置から目標位置へ補間
    Vector3 nextPos;
    nextPos.Lerp(currentPos, targetPos, positionLerp_); // 補間を使用して動作をスムーズに

    // カメラの位置を設定
    camera_->SetPosition(nextPos);

    // 次の回転を補間で計算
    Vector3 currentRot = camera_->GetRotate();
    Vector3 nextRot;
    nextRot.Lerp(currentRot, targetRot_, rotationLerp_);
    camera_->SetRotate(nextRot);

}

void FollowController::SnapToTarget()
{
	if (!camera_ or !targetPos_)
	{
		return;
	}

	Vector3 playerPos = targetPos_();
	Vector3 targetPos = playerPos + offset_;

	// 直接スナップ
	camera_->SetPosition(targetPos);
	camera_->SetRotate(targetRot_);
}
