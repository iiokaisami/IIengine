#include "GuardianAppearCameraController.h"

#include <Ease.h>

GuardianAppearCameraController::GuardianAppearCameraController(std::shared_ptr<IIEngine::Camera> camera, const std::function<Vector3()>& getGuardianPos, const std::function<Vector3()>& getFollowCameraPos, float moveDuration, float waitDuration, float returnDuration)
    :
    camera_(std::move(camera)),
    getGuardianPos_(std::move(getGuardianPos)),
    getFollowCameraPos_(std::move(getFollowCameraPos)),
    moveDuration_(moveDuration),
    waitDuration_(waitDuration),
    returnDuration_(returnDuration)
{}

void GuardianAppearCameraController::Update(float dt)
{
    if (!active_ || !camera_)
    {
        return;
    }

    switch (phase_)
    {
        //----------------------------------
        // Guardianへ移動
        //----------------------------------
    case Phase::MoveToGuardian:
    {
        timer_ += dt;

        float t = std::clamp(timer_ / moveDuration_, 0.0f, 1.0f);
        float e = Ease::InOutQuad(t);

        Vector3 camPos = Lerp(phaseFromPos_, phaseToPos_, e);
        camera_->SetPosition(camPos);

        // Guardianを見る
        Vector3 dir = guardianPos_ - camPos;

        float yaw = atan2(dir.x, dir.z);
        float horizontal = sqrt(dir.x * dir.x + dir.z * dir.z);
        float pitch = atan2(-dir.y, horizontal);

        camera_->SetRotate({ pitch, yaw, 0.0f });

        if (t >= 1.0f)
        {
            arrivedPos_ = camPos;
            arrivedRot_ = camera_->GetRotate();

            phase_ = Phase::Wait;
            timer_ = 0.0f;
        }

        break;
    }

    //----------------------------------
    // Guardianを見る
    //----------------------------------
    case Phase::Wait:
    {
        // タイマーを進める
        timer_ += dt;

        // 到着位置・回転を維持
        camera_->SetPosition(arrivedPos_);
        camera_->SetRotate(arrivedRot_);

        if (timer_ >= waitDuration_)
        {
            phase_ = Phase::Return;
			timer_ = 0.0f;

            phaseFromPos_ = camera_->GetPosition();

            phaseToPos_ = getFollowCameraPos_();

            returnStartRot_ = camera_->GetRotate();
            returnTargetRot_ = startRot_;
        }

        break;
    }

    //----------------------------------
    // Followへ戻る
    //----------------------------------
    case Phase::Return:
    {
        timer_ += dt;

        float t = std::clamp(timer_ / returnDuration_, 0.0f, 1.0f);
        float e = Ease::InOutQuad(t);

        Vector3 camPos = Lerp(phaseFromPos_, phaseToPos_, e);
        camera_->SetPosition(camPos);

        Vector3 rot = Lerp(returnStartRot_, returnTargetRot_, e);
        camera_->SetRotate(rot);

        if (t >= 1.0f)
        {
            camera_->SetPosition(phaseToPos_);
            camera_->SetRotate(startRot_);

            active_ = false;
            phase_ = Phase::Finished;

            if (onFinish_)
            {
                onFinish_();
            }
        }

        break;
    }

    default:
        break;
    }
}

void GuardianAppearCameraController::Start()
{
    if (active_)
    {
        return;
    }

    if (!camera_ || !getGuardianPos_ || !getFollowCameraPos_)
    {
        return;
    }

    active_ = true;
    timer_ = 0.0f;
    phase_ = Phase::MoveToGuardian;

    // 開始位置
    phaseFromPos_ = camera_->GetPosition();
    startRot_ = camera_->GetRotate();

    // Guardian位置を固定
    guardianPos_ = getGuardianPos_();

    // Follow位置を固定
    returnCameraPos_ = getFollowCameraPos_();

    // Guardianを少し斜め上から映す
    phaseToPos_ = guardianPos_ + Vector3{ 0.0f, 20.0f, -24.0f };
}
