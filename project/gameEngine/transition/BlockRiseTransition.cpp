#include "BlockRiseTransition.h"

#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979f
#endif

BlockRiseTransition::BlockRiseTransition(Mode mode) : mode_(mode)
{
    blocks_.reserve(rows_ * cols_);

    // グリッド生成
    for (uint32_t y = 0; y < rows_; ++y) 
    {
        for (uint32_t x = 0; x < cols_; ++x)
        {
            Block block;
            block.sprite = std::make_unique<Sprite>();
            block.sprite->Initialize("white.png", { 0,0 }, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });

            block.position = { x * blockSize_, y * blockSize_ };
            block.scale = { 0.0f, 0.0f };
            block.delay = ((float)x + (float)y * 0.1f + 1.0f) * blockInterval_;
            block.active = false;
            block.fallSpeed = 0.0f;
			block.verticalOffset = 0.0f;
			block.riseTimer = 0.0f;
            block.dropDelay = 0.0f;//((float)x * 1.5f + std::sin((float)y * 0.7f) * 2.0f + 1.0f) * blockInterval_; <-↓こっちは波状にバラバラ落ちていく
            block.riseDuration = 0.12f + (rand() % 100) * 0.005f; //0.15f + 0.07f * ((float)(rand() % 100) / 100.0f);
            block.isRising = false;
            block.isDropped = false;

            block.sprite->SetSize({ blockSize_, blockSize_ + 2.0f });
            block.sprite->SetColor({ 1,1,1,1 });

            blocks_.push_back(std::move(block));
        }
    }
}

void BlockRiseTransition::Start(std::function<void()> onSceneChange)
{
    onSceneChange_ = onSceneChange;
    sceneChanged_ = false;
    if (mode_ == Mode::DropOnly) 
    {
        state_ = State::Hold;
        timer_ = 0.0f;
    }
    else
    {
        state_ = State::Fill;
        timer_ = -blockInterval_;
    }

    if (mode_ == Mode::DropOnly)
    {
        // 全ブロックをすでに表示状態にして、覆われた状態から開始
        for (uint32_t y = 0; y < rows_; ++y)
        {
            for (uint32_t x = 0; x < cols_; ++x)
            {
                uint32_t index = y * cols_ + x;
                auto& b = blocks_[index];

                // 位置を正しいグリッドに補正
                b.position = { x * blockSize_, y * blockSize_ };
                b.scale = { blockSize_, blockSize_ + 2.0f };
                b.active = true;
                b.fallSpeed = 0.0f;
            
                b.sprite->SetPosition(b.position);
                b.sprite->SetSize(b.scale);
				b.sprite->Update();
            }
        }

        // 1フレーム目からすべて描画済み状態に
        state_ = State::Hold;
        timer_ = 0.0f;
    }
    else
    {
        state_ = State::Fill;
    }
}

void BlockRiseTransition::Update()
{
    for (auto& b : blocks_)
    {
		b.sprite->Update();
    }

    timer_ += 1.0f / 60.0f;

    switch (state_)
    {
    case State::Fill:
       
        for (auto& b : blocks_) 
        {
            if (timer_ > b.delay)
            {
                b.active = true;
                if (b.scale.x < blockSize_)
                {
                    b.scale.x += fillSpeed_;
                    b.scale.y += fillSpeed_;

                    b.sprite->SetPosition(b.position);
                    b.sprite->SetSize(b.scale);
					b.sprite->Update();
                }
            }
        }

        // 全ブロックが拡大完了
        if (std::all_of(blocks_.begin(), blocks_.end(),
            [](auto& b) { return b.scale.x >= 90.0f; }))
        {

            state_ = State::Hold;
            timer_ = 0.0f;

            if (onSceneChange_ && !sceneChanged_) 
            {
                onSceneChange_(); // ここでシーン切り替え
                sceneChanged_ = true;
            }
        }
        break;

    case State::Hold:
       

        if (timer_ > 0.3f) 
        { 
            // 少し待ってから落下開始
            state_ = State::Drop;
        }
        break;

    case State::Drop:

        for (auto& b : blocks_)
        {
            if (!b.active) continue;

            // 上昇開始判定
            if (!b.isRising && !b.isDropped && timer_ > b.dropDelay)
            {
                b.isRising = true;
                b.riseTimer = 0.0f;
                b.fallSpeed = 0.0f;
            }

            // 上昇中
            if (b.isRising)
            {
                float riseAmount = 32.0f;
                b.riseTimer += 1.0f / 60.0f;
                float t = b.riseTimer / b.riseDuration;
                if (t > 1.0f) t = 1.0f;
                b.verticalOffset = -riseAmount * std::sin(t * M_PI);

                if (b.riseTimer >= b.riseDuration)
                {
                    b.isRising = false;
                    b.isDropped = true;
                    b.verticalOffset = 0.0f;
                    b.fallSpeed = 1.2f + (rand() % 100) * 0.012f; // ->こっちは波状にバラバラ落ちていく 1.0f + static_cast<float>(rand() % 100) / 100.0f;
                }
            }
            // 落下
            if (b.isDropped)
            {
                b.position.y += b.fallSpeed * 10.0f;
            }

            b.sprite->SetPosition({ b.position.x, b.position.y + b.verticalOffset });
            b.sprite->Update();
        }

		// 全ブロックが画面外に落下完了
		if (std::all_of(blocks_.begin(), blocks_.end(),
			[](auto& b) { return b.position.y > 900.0f; }))
		{
			state_ = State::Done;
		}

        break;

	case State::Done:

		break;

    default:

        break;
    }
}

void BlockRiseTransition::Draw()
{
    for (auto& b : blocks_)
    {
        if (!b.active) continue;
       
        b.sprite->Draw();
    }
}

bool BlockRiseTransition::IsFinished() const
{
    return state_ == State::Done;
}
