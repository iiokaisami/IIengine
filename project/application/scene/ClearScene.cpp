#include "ClearScene.h"

#include "TimeManager.h"

using namespace IIEngine;

void ClearScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	camera_ = std::make_unique<SceneCamera>();
	camera_->Initialize();
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(camera_->GetCamera());
	cameraManager.AddCamera(camera_->GetCamera());
	cameraManager.SetActiveCamera(0);

	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	//プレイヤー
	pPlayer_ = std::make_unique<Player>();
	pPlayer_->Initialize();

	// フィールド
	pField_ = std::make_unique<Field>();
	pField_->Initialize();

	// スプライト
	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		auto sprite = std::make_unique<Sprite>();
		
		if (i == 0)
		{
			sprite->Initialize("clearLogo.png", { 0,0 }, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		} 
		else if (i == 1)
		{
			sprite->Initialize("reTry.png", rePos_, reColor_, { 0,0 });
		} 
		else if (i == 2)
		{
			sprite->Initialize("toTitle.png", toPos_, toColor_, { 0,0 });
		}
		else if(i == 3)
		{
			sprite->Initialize("endUI.png", rePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		}
		else if(i == 4)
		{
			sprite->Initialize("titleControllerUI.png", {0.0f,20.0f}, uiColor_, { 0,0 });
		}
		
		sprites_.push_back(std::move(sprite));
	}

	// シーン開始時にフェードイン
	transition_ = std::make_unique<IIEngine::BlockRiseTransition>(IIEngine::BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	transition_->Start(nullptr);

	// 選択肢の位置を初期化
	reBasePos_ = rePos_;
	toBasePos_ = toPos_;
	// 初期は Retry を選択
	selectedIndex_ = 0;                 
	selectedOption_ = MenuOption::None;
	prevStickX_ = 0.0f;

}

void ClearScene::Finalize()
{
	pPlayer_->Finalize();
	pField_->Finalize();

	// Object3dCommon に設定したデフォルトカメラを解除
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(nullptr);

	// SceneCamera
	if (camera_)
	{
		camera_->Finalize();
		camera_.reset();
	}
}

void ClearScene::Update()
{
	// トランジション更新
	if (isTransitioning_ && transition_)
	{
		transition_->Update();

		// トランジション終了判定
		if (transition_->IsFinished())
		{
			transition_.reset();
			isTransitioning_ = false;
		}

	}

	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

	// カメラ更新
	camera_->Update(dt);
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	// スプライト更新
	for (auto& sprite : sprites_)
	{
		sprite->Update();
	}

	// プレイヤー更新
	pPlayer_->ClearSceneUpdate();

	// フィールド更新
	pField_->Update();

	// パーティクル
	IIEngine::ParticleEmitter::Emit("goal", particlePosition_, 1);
	IIEngine::ParticleEmitter::Emit("petalGroup", petalPosition_, 1);


#ifdef USE_IMGUI

	// 透明度の更新

	ImGui::Begin("ClearScene");


	ImGui::SliderFloat3("cameraPosition", &cameraPosition_.x, -80.0f, 20.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate_.x, -3.14f, 3.14f);

	ImGui::SliderFloat3("particlePosition", &petalPosition_.x, -50.0f, 50.0f);

	ImGui::End();

#endif // USE_IMGUI

	// 選択中のスプライトだけ左右往復させる
	{
		const float speed = 2.0f;
		const float minX = 0.0f;
		const float maxX = 30.0f;
		const float mid = (minX + maxX) * 0.5f;
		const float amp = (maxX - minX) * 0.5f;

		// まず基準位置へ
		rePos_ = reBasePos_;
		toPos_ = toBasePos_;

		// 選択中の方だけ timer を進め、offset を更新する
		if (selectedIndex_ == 0) // Retry 選択中
		{
			retrySwayTimer_ += dt;
			retrySwayOffsetX_ = mid + amp * std::sinf(retrySwayTimer_ * speed);

		} else if (selectedIndex_ == 1) // Title 選択中
		{
			titleSwayTimer_ += dt;
			titleSwayOffsetX_ = mid + amp * std::sinf(titleSwayTimer_ * speed);

		}

		// 位置へ反映t
		rePos_.x += retrySwayOffsetX_;
		toPos_.x += titleSwayOffsetX_;

		sprites_[1]->SetPosition(rePos_);
		sprites_[2]->SetPosition(toPos_);

		Vector2 cursorPos = (selectedIndex_ == 0) ? rePos_ : toPos_;
		if (selectedIndex_ == 1)
		{
			cursorPos.x += uiOffset_;
		}
		sprites_[3]->SetPosition(cursorPos);

		// 透明度の上下
		const float alphaSpeed = 4.0f;
		time_ += dt;
		uiColor_.w = 0.5f * (1.0f + std::sinf(time_ * alphaSpeed));

		sprites_[4]->SetColor(uiColor_);
	}

	// !isTransitioning_ のときだけ受付
	if (!isTransitioning_)
	{
		auto* input = IIEngine::Input::GetInstance();

		const int menuCount = 2; // Retry, Title

		// パッドの左スティック左右
		leftPadStick_ = false;
		rightPadStick_ = false;

		if (input->IsPadConnected())
		{
			const float threshold = 0.6f;
			float x = input->GetLeftStick().x; // -1..1 想定

			// 左
			leftPadStick_ = (prevStickX_ >= -threshold && x < -threshold);
			// 右
			rightPadStick_ = (prevStickX_ <= threshold && x > threshold);

			prevStickX_ = x;
		} 
		else
		{
			prevStickX_ = 0.0f;
		}

		// 左へ移動
		if (input->TriggerKey(DIK_A) or input->TriggerKey(DIK_LEFT) or
			(input->IsPadConnected() &&
			(input->TriggerPadButton(ControllerButtonType::DPadLEFT) or leftPadStick_)))
		{
			selectedIndex_ = (selectedIndex_ - 1 + menuCount) % menuCount;
		}

		// 右へ移動
		if (input->TriggerKey(DIK_D) or input->TriggerKey(DIK_RIGHT) or
			(input->IsPadConnected() &&
			(input->TriggerPadButton(ControllerButtonType::DPadRIGHT) or rightPadStick_)))
		{
			selectedIndex_ = (selectedIndex_ + 1) % menuCount;
		}

		// 決定
		if (input->TriggerKey(DIK_RETURN) or input->TriggerKey(DIK_SPACE) or
			(input->IsPadConnected() &&
			input->PushPadButton(ControllerButtonType::A)))
		{
			selectedOption_ = (selectedIndex_ == 0) ? MenuOption::Retry : MenuOption::Title;
		}

		// 選択確定したらトランジション開始
		if (selectedOption_ != MenuOption::None)
		{
			transition_ = std::make_unique<IIEngine::BlockRiseTransition>();
			isTransitioning_ = true;

			const MenuOption decided = selectedOption_;
			selectedOption_ = MenuOption::None;

			transition_->Start([decided]
				{
					if (decided == MenuOption::Title)
					{
						IIEngine::SceneManager::GetInstance()->ChangeScene("TITLE");
					}
					else // Retry
					{
						IIEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
					}
				});
		}
	}
}

void ClearScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	// プレイヤー描画
	pPlayer_->Draw();

	// フィールド描画
	pField_->Draw();

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}

}

void ClearScene::DrawOverlayTop()
{
	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	// トランジション描画
	if (isTransitioning_ && transition_)
	{
		transition_->Draw();
	}
}
