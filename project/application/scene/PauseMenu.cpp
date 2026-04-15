#include "PauseMenu.h"

#include "TimeManager.h"

void PauseMenu::Initialize()
{
    options_ =
    {
        MenuOption::Resume,
        MenuOption::Restart,
        MenuOption::ReturnTitle
    };

    const char* files[] =
    {
        "pauseBG.png",
        "pauseResume.png",
        "pauseRestart.png",
        "pauseTitle.png",
        "pauseUI.png"
    };

    for (uint32_t i = 0; i < spriteNum_; i++)
    {
        Vector2 pos = { 0.0f, 0.0f };
        basePositions_.push_back(pos);

        auto sprite = std::make_unique<IIEngine::Sprite>();
        sprite->Initialize(files[i], pos);
        menuSprites_.push_back(std::move(sprite));
    }
}

void PauseMenu::Update()
{
    if (!isActive_)
    {
        return;
    }

    auto* input = IIEngine::Input::GetInstance();

    if (ignoreEscape_)
    {
        if (!input->PushKey(DIK_ESCAPE) or
           (IIEngine::Input::GetInstance()->IsPadConnected() &&
           (IIEngine::Input::GetInstance()->TriggerPadButton(ControllerButtonType::Select) or
            IIEngine::Input::GetInstance()->TriggerPadButton(ControllerButtonType::Start))))
        {
            ignoreEscape_ = false;
        }
        return;
    }

	// デルタタイム取得
    const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();
    animeTimer_ += dt;

	// アニメーションパラメータ
    const float selectOffsetX = 20.0f;        // 右にずれる量
    const float swayAmplitude = 5.0f;         // 揺れ幅
    const float swaySpeed = 6.0f;              // 揺れ速度
	// メニューインデックス定数
    constexpr int kMenuStartIndex = 1; // Resume
    constexpr int kMenuCount = 3; // Resume, Restart, Title
    constexpr int kCursorIndex = 4; // pauseUI.png

    // ESCでメニュー閉じる
    if (input->TriggerKey(DIK_ESCAPE) or
       (IIEngine::Input::GetInstance()->IsPadConnected() &&
       (IIEngine::Input::GetInstance()->TriggerPadButton(ControllerButtonType::Select) or
       IIEngine::Input::GetInstance()->TriggerPadButton(ControllerButtonType::Start))))
    {
        Close();
		return;
    }

    const int menuCount = static_cast<int>(options_.size());

    if (IIEngine::Input::GetInstance()->IsPadConnected())
    {
        const float threshold = 0.6f;          // メニュー用
        float y = input->GetLeftStick().y;     // -1..1 想定

        // 前フレーム threshold 未満 -> 今回 threshold 超え で上移動
        upPadStick = (prevStickY_ <= threshold && y > threshold);

        // 前フレーム -threshold より上 -> 今回 -threshold 未満 で下移動
        downPadStick = (prevStickY_ >= -threshold && y < -threshold);

        prevStickY_ = y;
    } 
    else
    {
        prevStickY_ = 0.0f;
    }

    // 上移動
    if (input->TriggerKey(DIK_W) or input->TriggerKey(DIK_UP) or
       (IIEngine::Input::GetInstance()->IsPadConnected() &&
        (IIEngine::Input::GetInstance()->TriggerPadButton(ControllerButtonType::DPadUP) or 
         upPadStick)))
    {
        selectedIndex_ = (selectedIndex_ - 1 + menuCount) % menuCount;
    }

    // 下移動
    if (input->TriggerKey(DIK_S) or input->TriggerKey(DIK_DOWN) or
       (IIEngine::Input::GetInstance()->IsPadConnected() &&
       (IIEngine::Input::GetInstance()->TriggerPadButton(ControllerButtonType::DPadDOWN) or
        downPadStick)))
    {
        selectedIndex_ = (selectedIndex_ + 1) % menuCount;
    }

    // 決定
    if (input->TriggerKey(DIK_RETURN) or input->TriggerKey(DIK_SPACE) or
       (IIEngine::Input::GetInstance()->IsPadConnected() &&
        IIEngine::Input::GetInstance()->PushPadButton(ControllerButtonType::A)))
    {
        selectedOption_ = options_[selectedIndex_];
    }

    // メニュー文字スプライト更新
    for (uint32_t i = 0; i < kMenuCount; ++i)
    {
        const int spriteIndex = kMenuStartIndex + i;

        Vector2 pos = basePositions_[spriteIndex];

        if (i == selectedIndex_)
        {
            float sway = std::sin(animeTimer_ * swaySpeed) * swayAmplitude;
            pos.x += selectOffsetX + sway;
        }

        menuSprites_[spriteIndex]->SetPosition(pos);
    }



	// UI位置更新
    constexpr float kCursorSpacing = 100.0f;

    Vector2 cursorPos = basePositions_[kCursorIndex];
    cursorPos.y += kCursorSpacing * selectedIndex_;
    menuSprites_[kCursorIndex]->SetPosition(cursorPos);


	// スプライト更新
    for (auto& sprite : menuSprites_)
    {
        sprite->Update();
	}


}

void PauseMenu::Draw()
{
    if (!isActive_) return;

    for (auto& sprite : menuSprites_)
    {
        sprite->Draw();
    }
}

void PauseMenu::Open()
{
    isActive_ = true;
    ignoreEscape_ = true;
    selectedIndex_ = 0;
    selectedOption_ = MenuOption::None;
}

void PauseMenu::Close()
{
    isActive_ = false;
}


PauseMenu::MenuOption PauseMenu::GetSelectedOption() const
{
    return selectedOption_;
}

PauseMenu::MenuOption PauseMenu::ConsumeSelectedOption()
{
    MenuOption ret = selectedOption_;
    selectedOption_ = MenuOption::None;
    return ret;
}
