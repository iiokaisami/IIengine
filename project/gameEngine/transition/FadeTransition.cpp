#include "FadeTransition.h"

#include <algorithm>
#include <iostream>
#include "TransitionManager.h"

FadeTransition::FadeTransition(Mode mode) : mode_(mode) 
{
    fadeSprite_ = std::make_unique<Sprite>();
    fadeSprite_->Initialize("white.png", { 0, 0 }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0, 0 });
    Vector2 size = fadeSprite_->GetSize();
    size.x = 1280.0f;
    size.y = 720.0f;
	fadeSprite_->SetSize(size);

}

void FadeTransition::Start(std::function<void()> onSceneChange)
{
    onSceneChange_ = onSceneChange;

    if (mode_ == Mode::FadeInOnly)
    {
        state_ = State::FadeIn;
        alpha_ = 1.0f;
    } 
    else
    {
        state_ = State::FadeOut;
        alpha_ = 0.0f;
    }
}

void FadeTransition::Update()
{
	fadeSprite_->Update();

    switch (state_) 
    {
    case State::FadeOut:
        alpha_ += speed_;
        if (alpha_ >= 1.0f) 
        {
            alpha_ = 1.0f;
            if (onSceneChange_) onSceneChange_(); 
            state_ = State::FadeIn;
        }
        break;

    case State::FadeIn:
        alpha_ -= speed_;
        if (alpha_ <= 0.0f)
        {
            alpha_ = 0.0f;
            state_ = State::Done;
        }
        break;

    default:
        
        break;
    }
}

void FadeTransition::Draw()
{
    fadeSprite_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });
	fadeSprite_->Draw();
}

bool FadeTransition::IsFinished() const
{
    return state_ == State::Done;
}