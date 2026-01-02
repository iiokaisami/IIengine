#include "FollowController.h"

#include <Camera.h>

FollowController::FollowController(std::shared_ptr<Camera> camera, std::function<Vector3()> targetPos, float positionLerp, float rotationLerp, Vector3 targetRot)
	: camera_(camera),
      targetPos_(targetPos),
      positionLerp_(positionLerp),
	  rotationLerp_(rotationLerp),
	  targetRot_(targetRot)
{}

void FollowController::Update(float dt)
{
	dt;
	// 無効またはカメラがない場合は処理しない
	if(!active_ or !camera_)
	{
		return;
	}

	// 追尾対象の位置を取得
	Vector3 playerPos = targetPos_();
	// 固定のオフセット 
	Vector3 targetPos = playerPos + offset_;
	// 現在のカメラ位置を取得
	Vector3 currentPos = camera_->GetPosition();
	// 次の位置を補間で計算
	Vector3 nextPos;
	nextPos.Lerp(currentPos, targetPos, positionLerp_);
	// カメラの位置を設定
	camera_->SetPosition(nextPos);

	// 次の回転を補間で計算
	Vector3 currentRot = camera_->GetRotate();
	Vector3 nextRot;
	nextRot.Lerp(currentRot, targetRot_, rotationLerp_);
	// カメラの回転を設定
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
