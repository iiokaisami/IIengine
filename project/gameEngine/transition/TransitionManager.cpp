#include "TransitionManager.h"

namespace IIEngine
{

    void TransitionManager::Start(std::unique_ptr<BaseTransition> transition, std::function<void()> onSceneChange)
    {
        transition_ = std::move(transition);
        transition_->Start(onSceneChange);
    }

    void TransitionManager::Update()
    {
        // トランジション中でなければ何もしない
        if (!transition_) return;

        transition_->Update();

        if (transition_->IsFinished())
        {
            transition_.reset();
        }
    }

    void TransitionManager::Draw()
    {
        if (transition_)
        {
            transition_->Draw();
        }
    }

    bool TransitionManager::IsRunning() const
    {
        return transition_ != nullptr;
    }
}