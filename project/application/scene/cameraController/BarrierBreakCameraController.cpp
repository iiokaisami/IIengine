#include "BarrierBreakCameraController.h"

#include <Ease.h>

BarrierBreakCameraController::BarrierBreakCameraController(std::shared_ptr<IIEngine::Camera> camera, std::function<Vector3()> targetPos, std::function<Vector3()> returnPos, std::function<bool()> isBarrierBroken)
	:camera_(std::move(camera)),
	 getPlayerPos_(std::move(targetPos)),
	 getGoalPos_(std::move(returnPos)),
	isBarrierBroken_(std::move(isBarrierBroken))
{
}

void BarrierBreakCameraController::Update(float dt)
{
    if (!active_ or !camera_ or !getPlayerPos_ or !getGoalPos_)
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

        Vector3 goalPos = getGoalPos_();
        LookAt(camPos, goalPos);

        if (t >= 1.0f)
        {
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
        // Goalを注視し続ける
        Vector3 goalPos = getGoalPos_();
        Vector3 camPos = goalPos + goalViewOffset_;
        camera_->SetPosition(camPos);
        LookAt(camPos, goalPos);

        // バリア破壊完了で戻り開始
        if (isBarrierBroken_ && isBarrierBroken_())
        {
            phase_ = Phase::ReturnToPlayer;
            timer_ = 0.0f;

            phaseFromPos_ = camera_->GetPosition();
            Vector3 playerPos = getPlayerPos_();
            phaseToPos_ = playerPos + playerViewOffset_;
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

        Vector3 playerPos = getPlayerPos_();
        LookAt(camPos, playerPos);

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
    if (!camera_ or !getPlayerPos_ or !getGoalPos_)
    {
        return;
    }

    active_ = true;
    phase_ = Phase::ToGoal;
    timer_ = 0.0f;
    arrivedGoalSignaled_ = false;

    // シームレス開始：今のカメラ位置からGoal注視構図へ
    phaseFromPos_ = camera_->GetPosition();

    Vector3 goalPos = getGoalPos_();
    phaseToPos_ = goalPos + goalViewOffset_;
}

void BarrierBreakCameraController::LookAt(const Vector3& eye, const Vector3& target)
{
    Vector3 dir = (target - eye);

    float yaw = std::atan2(dir.x, dir.z);
    float horizontalDist = std::sqrt(dir.x * dir.x + dir.z * dir.z);
    float pitch = std::atan2(-dir.y, horizontalDist);

    camera_->SetRotate({ pitch, yaw, 0.0f });
}
