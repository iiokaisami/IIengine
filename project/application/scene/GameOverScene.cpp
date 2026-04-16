#include "GameOverScene.h"

#include <Ease.h>

#include "TimeManager.h"

using namespace IIEngine;

void GameOverScene::Initialize()
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

	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		auto sprite = std::make_unique<Sprite>();
		
		if (i == 0)
		{
			sprite->Initialize("gameOverLogo.png", { 0,0 }, {1.0f,1.0f,1.0f,1.0f}, { 0,0 });
		}
		else if (i == 1)
		{
			sprite->Initialize("reTry.png", rePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		}
		else if (i == 2)
		{
			sprite->Initialize("toTitle.png", toPos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		}
		else if (i == 3)
		{
			sprite->Initialize("endUI.png", rePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
		} else if (i == 4)
		{
			sprite->Initialize("titleControllerUI.png", { 0.0f,20.0f }, uiColor_, { 0,0 });
		}

		sprites_.push_back(std::move(sprite));
	}


	// 衝突判定
	colliderManager_ = ColliderManager::GetInstance();
	colliderManager_->Initialize();

	// プレイヤー
	pPlayer_ = std::make_unique<Player>();
	pPlayer_->Initialize();
	pPlayer_->SetPosition({ 0.0f,0.0f,0.0f });
	pPlayer_->SetIsCanMove(false); // 移動不可

	// エネミー
	pEnemyManager_ = std::make_unique<EnemyManager>();
	pEnemyManager_->GameOverEnemyInit();


	// カメラのシェイク
	camera_->AddController(std::make_unique<ShakeController>(
		camera_->GetCamera(),
		[this]() -> bool {
			bool hit = pPlayer_->IsHitMoment();
			if (hit) pPlayer_->SetHitMoment(false); // consume
			return hit;
		},
		0.3f, // duration
		0.5f  // amplitude
	));

	// シーン開始時にフェードイン
	transition_ = std::make_unique<IIEngine::BlockRiseTransition>(IIEngine::BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	transition_->Start(nullptr);

}

void GameOverScene::Finalize()
{
	pPlayer_->Finalize();
	pEnemyManager_->Finalize();

	// Object3dCommon に設定したデフォルトカメラを解除
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(nullptr);

	// SceneCamera
	if (camera_)
	{
		camera_->Finalize();
		camera_.reset();
	}
}

void GameOverScene::Update()
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

	for (auto& sprite : sprites_)
	{
		sprite->Update();

	}

	// 当たり判定チェック
	colliderManager_->CheckAllCollision();

	// プレイヤーの更新
	pPlayer_->SetIsNoiseActive(false); // ゲームオーバーシーンではノイズを常にオフにする
	pPlayer_->Update();
	// プレイヤーの位置をエネミーマネージャーにセット
	pEnemyManager_->SetPlayerPosition(pPlayer_->GetPosition());

	// カメラの更新(シェイク、追尾、引き)
	if (!pPlayer_->IsDead())
	{
		camera_->SetRotate(cameraRotate_);
		camera_->SetPosition(cameraPosition_);
		CameraShake();
	}

	// エネミーの更新
	pEnemyManager_->GameOverEnemyUpdate();



#ifdef USE_IMGUI

	// 透明度の更新

	ImGui::Begin("GameOverScene");

	//ImGui::SliderFloat4("transparent", &color_.x, 0.0f, 1.0f);

	ImGui::SliderFloat3("cameraPosition", &cameraPosition_.x, -50.0f, 20.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate_.x, -3.14f, 3.14f);

	
	ImGui::End();

	pEnemyManager_->ImGuiDraw();

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
		} else
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
					} else // Retry
					{
						IIEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
					}
				});
		}
	}

}

void GameOverScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	pPlayer_->Draw();

	pEnemyManager_->Draw();

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}

}

void GameOverScene::DrawOverlayTop()
{
	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	// トランジション描画
	if (isTransitioning_ && transition_)
	{
		transition_->Draw();
	}
}

void GameOverScene::CameraShake()
{
	// アクティブカメラの情報を取得
	auto activeCamera = cameraManager.GetActiveCamera();

	// プレイヤーがヒットした場合にカメラをシェイク
	if (pPlayer_->IsHitMoment())
	{
		// アクティブなカメラを取得
		if (activeCamera)
		{
			// カメラをシェイク (持続時間,振幅)
			activeCamera->StartShake(0.3f, 0.6f);

			// ヒットフラグをリセット
			pPlayer_->SetHitMoment(false);
		}
	}

	// シェイク
	if (activeCamera)
	{
		activeCamera->UpdateShake(1.0f / 60.0f);
	}
}