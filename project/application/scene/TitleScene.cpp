#include "TitleScene.h"

#include <cmath>
#include <ModelManager.h>

void TitleScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	camera_ = std::make_shared<Camera>();

	cameraPosition_ = camera_->GetRotate();
	cameraRotate_ = camera_->GetPosition();
	cameraPosition_.y = 70.0f;
	cameraPosition_.z = -15.0f;
	cameraRotate_.x = 1.2f;

	cameraManager.AddCamera(camera_);
	cameraManager.SetActiveCamera(0);
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_);


	// 衝突判定
	colliderManager_ = ColliderManager::GetInstance();
	colliderManager_->Initialize();

	// プレイヤー
	pPlayer_ = std::make_unique<Player>();
	pPlayer_->Initialize();
	pPlayer_->SetAutoControl(true); // デモモードに設定

	// エネミー
	pEnemyManager_ = std::make_unique<EnemyManager>();
	pEnemyManager_->TitleEnemyInit();

	// フィールド
	pField_ = std::make_unique<Field>();
	pField_->Initialize();


	// --- 2Dスプライト ---
	for (uint32_t i = 0; i < spriteNum_; ++i)
	{
		Sprite* sprite = new Sprite();

		if (i == 0)
		{
			
			sprite->Initialize("titleUI.png", { 0,0 }, color_, { 0,0 });
		}
		sprites.push_back(sprite);

	}

	// --- サウンド ---
	soundData_ = Audio::GetInstance()->LoadWav("fanfare.wav");
	//Audio::GetInstance()->PlayWave(soundData_, false, 0.2f);
	soundData2_ = Audio::GetInstance()->LoadWav("BGM.wav");
	//Audio::GetInstance()->PlayWave(soundData2_, true, 0.2f);


	// 環境マップ
	cubeMapPath_ = "resources/images/studio.dds";
	TextureManager::GetInstance()->LoadTexture(cubeMapPath_);
	cubeSrvIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(cubeMapPath_);
	cubeHandle_ = TextureManager::GetInstance()->GetSrvManager()->GetGPUDescriptorHandle(cubeSrvIndex_);
	
		
	// シーン開始時にフェードイン
	transition_ = std::make_unique<BlockRiseTransition>(BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	transition_->Start(nullptr);
}

void TitleScene::Finalize()
{
	pPlayer_->Finalize();
	pEnemyManager_->Finalize();
	pField_->Finalize();

	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
	sprites.clear();

	Audio::GetInstance()->SoundUnload(Audio::GetInstance()->GetXAudio2(), &soundData_);
	Audio::GetInstance()->SoundUnload(Audio::GetInstance()->GetXAudio2(), &soundData2_);

	cameraManager.RemoveCamera(0);
}

void TitleScene::Update()
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

	camera_->Update();
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);

	// 当たり判定チェック
	colliderManager_->CheckAllCollision();

	// プレイヤー
	pPlayer_->Update();
	// プレイヤーの位置をエネミーマネージャーにセット
	pEnemyManager_->SetPlayerPosition(pPlayer_->GetPosition());

	// カメラの更新(シェイク、追尾、引き)
	CameraUpdate();
	
	// エネミー
	pEnemyManager_->TitleEnemyUpdate();
	
	// フィールド
	pField_->Update();


	for (Sprite* sprite : sprites)
	{
		sprite->Update();

		sprite->SetColor(color_);

	}



#ifdef USE_IMGUI

	// 透明度の更新

	ImGui::Begin("TitleScene");

	ImGui::SliderFloat4("transparent", &color_.x, 0.0f, 1.0f);

	ImGui::SliderFloat3("cameraPosition", &cameraPosition_.x, -70.0f, 20.0f);
	ImGui::SliderFloat3("cameraRotate", &cameraRotate_.x, -3.14f, 3.14f);

	if (ImGui::Button("Terrain Draw"))
	{
		isTerrainDraw = !isTerrainDraw;
	}

	// ライトの設定
	ImGui::Checkbox("Enable Lighting", &enableLighting);

	ImGui::Checkbox("Directional Light", &enableDirectionalLight);
	if (enableDirectionalLight)
	{
		ImGui::ColorEdit3("Directional Light Color", &directionalLightColor.x);
		ImGui::SliderFloat3("Directional Light Direction", &directionalLightDirection.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Directional Light Intensity", &directionalLightIntensity, 0.0f, 10.0f);
	}

	ImGui::Checkbox("Point Light", &enablePointLight);
	if (enablePointLight)
	{
		ImGui::ColorEdit3("Point Light Color", &pointLightColor.x);
		ImGui::SliderFloat3("Point Light Position", &pointLightPosition.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Point Light Intensity", &pointLightIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Point Light Radius", &pointLightRadius, 0.0f, 50.0f);
		ImGui::SliderFloat("Point Light Decay", &pointLightDecay, 0.0f, 1.0f);
	}

	ImGui::Checkbox("Spot Light", &enableSpotLight);
	if (enableSpotLight)
	{
		ImGui::ColorEdit3("Spot Light Color", &spotLightColor.x);
		ImGui::SliderFloat3("Spot Light Position", &spotLightPosition.x, -20.0f, 20.0f);
		ImGui::SliderFloat3("Spot Light Direction", &spotLightDirection.x, -1.0f, 1.0f);
		ImGui::SliderFloat("Spot Light Intensity", &spotLightIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Spot Light Distance", &spotLightDistance, 0.0f, 50.0f);
		ImGui::SliderFloat("Spot Light Decay", &spotLightDecay, 0.0f, 1.0f);
		ImGui::SliderFloat("Spot Light Cons Angle", &spotLightConsAngle, 0.0f, 10.0f);
		ImGui::SliderFloat("Spot Light Cos FalloffStart", &spotLightCosFalloffStart, 0.0f, 1.0f);
	}

	ImGui::Checkbox("Enable Environment", &enableEnvironment);
	if (enableEnvironment)
	{
		ImGui::SliderFloat("Environment Strength", &environmentStrength_, 0.0f, 1.0f);
	}


	ImGui::End();

	pPlayer_->ImGuiDraw();
	pEnemyManager_->ImGuiDraw();
	pField_->ImGuiDraw();

#endif // USE_IMGUI

	

	if (Input::GetInstance()->TriggerKey(DIK_RETURN))
	{
		// トランジション開始
		transition_ = std::make_unique<BlockRiseTransition>();
		isTransitioning_ = true;
		transition_->Start([]
			{
				// フェードアウト後にシーン切り替え
				SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
			});
	}

	if (Input::GetInstance()->TriggerKey(DIK_Q))
	{
		Audio::GetInstance()->SoundStop(soundData_);
	}
	if (Input::GetInstance()->TriggerKey(DIK_E))
	{
		Audio::GetInstance()->SoundStop(soundData2_);
	}
}

void TitleScene::Draw()
{
	// 描画前処理(Object)
	Object3dCommon::GetInstance()->CommonDrawSetting();

	// プレイヤー
	pPlayer_->Draw();
	// エネミー
	pEnemyManager_->Draw();
	// フィールド
	pField_->Draw();

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	for (Sprite* sprite : sprites)
	{
		sprite->Draw();
	}

	// トランジション描画
	if (isTransitioning_ && transition_) 
	{
		transition_->Draw();
	}
	
}

void TitleScene::CameraUpdate()
{
	// カメラのシェイク
	CameraShake();

	// カメラの追従
	CameraFollow();
}

void TitleScene::CameraShake()
{
	// アクティブカメラの情報を取得
	auto activeCamera = cameraManager.GetActiveCamera();
	if (activeCamera)
	{
		auto viewMatrix = activeCamera->GetViewMatrix();
	}

	// プレイヤーがヒットした場合にカメラをシェイク
	if (pPlayer_->IsHitMoment())
	{
		// アクティブなカメラを取得
		if (activeCamera)
		{
			// カメラをシェイク (持続時間,振幅)
			activeCamera->StartShake(0.3f, 0.5f);

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

void TitleScene::CameraFollow()
{
	if (!camera_ or !pPlayer_)
	{
		return;
	}

	Vector3 playerPos = pPlayer_->GetPosition();

	// 固定のオフセット（プレイヤーから見たカメラ位置）
	Vector3 offset = { 0.0f, 80.0f, -20.0f };  // Y: 高さ、Z: 後方

	// 追従先の位置・回転
	Vector3 targetPos = playerPos + offset;
	Vector3 targetRot = { 2.0f, 0.0f, 0.0f };  // やや下向き

	// 滑らかに補間して追従
	Vector3 currentPos = camera_->GetPosition();
	Vector3 nextPos;
	nextPos.Lerp(currentPos, targetPos, 0.8f);

	Vector3 currentRot = camera_->GetRotate();
	Vector3 nextRot;
	nextRot.Lerp(currentRot, targetRot, 0.25f);

	camera_->SetPosition(nextPos);
	camera_->SetRotate(nextRot);

}

