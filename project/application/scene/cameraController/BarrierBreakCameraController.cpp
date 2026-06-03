#include "BarrierBreakCameraController.h"

#include <Ease.h>

BarrierBreakCameraController::BarrierBreakCameraController(std::shared_ptr<IIEngine::Camera> camera, const std::function<Vector3()>& targetPos, const std::function<Vector3()>& returnPos, const std::function<Vector3()>& getFollowCamPos, const std::function<bool()>& isBarrierBroken)
	:camera_(std::move(camera)),
	 getPlayerPos_(std::move(targetPos)),
	 getGoalPos_(std::move(returnPos)),
     getFollowCamPos_(std::move(getFollowCamPos)),
	 isBarrierBroken_(std::move(isBarrierBroken))
{
}

void BarrierBreakCameraController::Update(float dt)
{
    if (!active_ or !camera_ or !getPlayerPos_ or !getGoalPos_ or !getFollowCamPos_)
    {
        return;
    }

    switch (phase_)
    {
    case Phase::ToGoal:
    {
        timer_ += dt;
        float t = std::clamp(timer_ / toGoalDuration_, 0.0f, 1.0f);
        float te = Ease::InOutQuad(t);

        Vector3 camPos = Lerp(phaseFromPos_, phaseToPos_, te);
        camera_->SetPosition(camPos);

        // 回転
        Vector3 rot = Lerp(startRot_, goalTargetRot_, te);
        camera_->SetRotate(rot);

        if (t >= 1.0f)
        {
            // 到達位置を保持
            arrivedGoalPos_ = camera_->GetPosition();

            // 最終回転を確定
            camera_->SetRotate(goalTargetRot_);

            // Goalに到達した瞬間に一度だけ通知
            if (!arrivedGoalSignaled_)
            {
                arrivedGoalSignaled_ = true;
                if (onArriveGoal_) onArriveGoal_();
            }

            phase_ = Phase::WaitBroken;
            timer_ = 0.0f;
        }
        break;
    }
    case Phase::WaitBroken:
    {
        // その場で止める
        camera_->SetPosition(arrivedGoalPos_);
        camera_->SetRotate(goalTargetRot_);


        // バリア破壊完了後だけ timer を進める
        if (isBarrierBroken_ && isBarrierBroken_())
        {
            timer_ += dt;
        }

        if (timer_ >= waitTime_)
        {
            phase_ = Phase::ReturnToPlayer;
            timer_ = 0.0f;

            phaseFromPos_ = camera_->GetPosition();

            // 戻り先：Follow位置
            Vector3 followPos = getFollowCamPos_();
            phaseToPos_ = followPos;

            // 戻り回転補間
            returnStartRot_ = camera_->GetRotate();
            returnTargetRot_ = initialRot_;
        }

        break;
    }
    case Phase::ReturnToPlayer:
    {
        timer_ += dt;
        float t = std::clamp(timer_ / returnDuration_, 0.0f, 1.0f);
        float te = Ease::InOutQuad(t);

        Vector3 camPos = Lerp(phaseFromPos_, phaseToPos_, te);
        camera_->SetPosition(camPos);

        Vector3 rot = Lerp(returnStartRot_, returnTargetRot_, te);
        camera_->SetRotate(rot);

        if (t >= 1.0f)
        {
            phase_ = Phase::Finished;
            active_ = false;
            camera_->SetRotate(returnTargetRot_);

            if (onFinish_) onFinish_();
        }
        break;
    }
    default:
        break;
    }
}

void BarrierBreakCameraController::Start()
{
    if (!camera_ or !getPlayerPos_ or !getGoalPos_ or !getFollowCamPos_)
    {
        return;
    }

    active_ = true;
    phase_ = Phase::ToGoal;
    timer_ = 0.0f;
    arrivedGoalSignaled_ = false;

    // 開始位置：現在のカメラ位置
    phaseFromPos_ = camera_->GetPosition();

    // 目標：ゴール位置 + 固定オフセット
    Vector3 goalCamPos = getGoalPos_() + goalExtraOffset_;
    phaseToPos_ = goalCamPos;

    // 回転補間の準備
    startRot_ = camera_->GetRotate();
    initialRot_ = camera_->GetRotate();

}
