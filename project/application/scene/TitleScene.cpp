#include "TitleScene.h"

#include <cmath>
#include <ModelManager.h>

#include "TimeManager.h"

using namespace IIEngine;

void TitleScene::Initialize()
{
	// 必ず先頭でカメラを全クリア
	cameraManager.ClearAllCameras();

	camera_ = std::make_unique<SceneCamera>();
	camera_->Initialize();

	cameraPosition_ = camera_->GetRotate();
	cameraRotate_ = camera_->GetPosition();
	cameraPosition_.y = 70.0f;
	cameraPosition_.z = -15.0f;
	cameraRotate_.x = 1.2f;

	cameraManager.AddCamera(camera_->GetCamera());
	cameraManager.SetActiveCamera(0);
	IIEngine::Object3dCommon::GetInstance()->SetDefaultCamera(camera_->GetCamera());


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
		auto sprite = std::make_unique<Sprite>();

		if (i == 0)
		{
			
			sprite->Initialize("titleLogo.png", { 0,0 }, {1.0f,1.0f,1.0f,1.0f}, {0,0});
		}
		if(i == 1)
		{
			sprite->Initialize("titleUI.png", { 0,0 }, color_, { 0,0 });
		}

		sprites_.push_back(std::move(sprite));

	}


	// 環境マップ
	cubeMapPath_ = "resources/images/studio.dds";
	TextureManager::GetInstance()->LoadTexture(cubeMapPath_);
	cubeSrvIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(cubeMapPath_);
	cubeHandle_ = TextureManager::GetInstance()->GetSrvManager()->GetGPUDescriptorHandle(cubeSrvIndex_);
	
	// カメラの追従
	camera_->AddController(std::make_unique<FollowController>(
		camera_->GetCamera(),
		[this]() -> Vector3 { return pPlayer_->GetPosition(); }, // プレイヤー位置を供給
		[this]() -> Vector3 { return pPlayer_->GetVelocity(); }, // プレイヤーの速度を供給
		0.8f, 0.25f,
		Vector3{ 0.4f, 0.0f, 0.0f }
	));

	// カメラのシェイク
	camera_->AddController(std::make_unique<ShakeController>(
		camera_->GetCamera(),
		[this]() -> bool
		{
			bool hit = pPlayer_->IsHitMoment();
			if (hit) pPlayer_->SetHitMoment(false); // consume
			return hit;
		},
		duration_,
		amplitude_
	));
		
	// シーン開始時にフェードイン
	transition_ = std::make_unique<IIEngine::BlockRiseTransition>(IIEngine::BlockRiseTransition::Mode::DropOnly);
	isTransitioning_ = true;
	transition_->Start(nullptr);

	// ノイズを0に
	PostEffectManager::GetInstance()->GetPassAs<NoisePass>("Noise")->SetIntensity(0.0f);
	// ChromaticPulseをoffに
	PostEffectManager::GetInstance()->GetPassAs<ChromaticPulsePass>("ChromaticPulse")->SetActive(false);
	// スキャンラインをoffに
	PostEffectManager::GetInstance()->GetPassAs<ScanlinePass>("Scanline")->SetActive(false);
	
}

void TitleScene::Finalize()
{
	pPlayer_->Finalize();
	pEnemyManager_->Finalize();
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

	// delta
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();


	// 透明度の上下
	const float alphaSpeed = 4.0f;
	time_ += dt;
	color_.w = 0.5f * (1.0f + std::sinf(time_ * alphaSpeed)); 


	// 当たり判定チェック
	colliderManager_->CheckAllCollision();

	// プレイヤー
	pPlayer_->Update();
	// プレイヤーの位置をエネミーマネージャーにセット
	pEnemyManager_->SetPlayerPosition(pPlayer_->GetPosition());

	// カメラの更新(シェイク、追尾、引き)
	camera_->Update(dt);
	camera_->SetPosition(cameraPosition_);
	camera_->SetRotate(cameraRotate_);
	
	// エネミー
	pEnemyManager_->TitleEnemyUpdate();
	
	// フィールド
	pField_->Update();


	for (auto& sprite : sprites_)
	{
		sprite->Update();

	}

	sprites_[1]->SetColor(color_);



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

	

	if (IIEngine::Input::GetInstance()->TriggerKey(DIK_RETURN) && !isTransitioning_ or
		(IIEngine::Input::GetInstance()->IsPadConnected() &&
		IIEngine::Input::GetInstance()->PushPadButton(ControllerButtonType::A)))
	{
		// トランジション開始
		transition_ = std::make_unique<IIEngine::BlockRiseTransition>();
		isTransitioning_ = true;
		transition_->Start([]
			{
				// フェードアウト後にシーン切り替え
				IIEngine::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
			});
	}
}

void TitleScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	// プレイヤー
	pPlayer_->Draw();
	// エネミー
	pEnemyManager_->Draw();
	// フィールド
	pField_->Draw();

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}

	// トランジション描画
	if (isTransitioning_ && transition_) 
	{
		transition_->Draw();
	}
	
}
