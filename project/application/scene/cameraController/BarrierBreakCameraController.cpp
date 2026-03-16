#include "BarrierBreakCameraController.h"

#include <Ease.h>

BarrierBreakCameraController::BarrierBreakCameraController(std::shared_ptr<IIEngine::Camera> camera, std::function<Vector3()> targetPos, std::function<Vector3()> returnPos, std::function<Vector3()> getFollowCamPos, std::function<bool()> isBarrierBroken)
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

        Vector3 camPos = LerpXZ(phaseFromPos_, phaseToPos_, te, fixedY_);
        camera_->SetPosition(camPos);

        if (t >= 1.0f)
        {
            // 到達位置を保持
            arrivedGoalPos_ = camera_->GetPosition();

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
            followPos.y = fixedY_;
            phaseToPos_ = followPos + cameraOffset_;
        }
        break;
    }

    case Phase::ReturnToPlayer:
    {
        timer_ += dt;
        float t = std::clamp(timer_ / returnDuration_, 0.0f, 1.0f);
        float te = Ease::InOutQuad(t);

        Vector3 camPos = LerpXZ(phaseFromPos_, phaseToPos_, te, fixedY_);
        camera_->SetPosition(camPos);

        if (t >= 1.0f)
        {
            phase_ = Phase::Finished;
            active_ = false;

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

    // 角度を固定
    fixedRot_ = camera_->GetRotate();
    camera_->SetRotate(fixedRot_);

    // y固定
    fixedY_ = camera_->GetPosition().y;

    // 開始位置：現在のカメラ位置
    phaseFromPos_ = camera_->GetPosition();

    // 目標：ゴール位置 + 固定オフセット
    Vector3 goalCamPos = getGoalPos_() + cameraOffset_;
    goalCamPos.y = fixedY_;
    phaseToPos_ = goalCamPos;
}

void BarrierBreakCameraController::LookAt(const Vector3& eye, const Vector3& target)
{
    Vector3 dir = (target - eye);

    float yaw = std::atan2(dir.x, dir.z);
    float horizontalDist = std::sqrt(dir.x * dir.x + dir.z * dir.z);
    float pitch = std::atan2(-dir.y, horizontalDist);

    camera_->SetRotate({ pitch, yaw, 0.0f });
}
