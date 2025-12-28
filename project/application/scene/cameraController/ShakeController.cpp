#include "ShakeController.h"

#include <Camera.h>

ShakeController::ShakeController(std::shared_ptr<Camera> camera, std::function<bool()> consumeHit, float defaultDuration, float defaultAmp)
    : camera_(camera),
      consumeHit_(consumeHit),
      duration_(defaultDuration),
	  amplitude_(defaultAmp)
{}

void ShakeController::Update(float dt)
{
    if (!active_ || !camera_)
    {
        return;
    }

    if (consumeHit_ && consumeHit_()) 
    {
        // シェイクスタート
        camera_->StartShake(duration_, amplitude_);
    }

    camera_->UpdateShake(dt);
}
