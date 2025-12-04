#include "MyGame.h"

#include <thread>

void MyGame::Initialize()
{
	Framework::Initialize();

	// シーンマネージャに最初のシーンをセット
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());

	// 最初のシーンを設定
	SceneManager::GetInstance()->ChangeScene("TITLE");


	// モデルの読み込み処理をスレッドで実行
	std::thread loadModelThread(&MyGame::LoadModel, this);
	std::thread loadAudioThread(&MyGame::LoadSound, this);

	// スレッドの終了を待つ
	loadModelThread.join(); 
	loadAudioThread.join();

	// パーティクルグループの生成
	particleManager->CreateParticleGroup("slash", "resources/images/gradationLine.png", "plane.obj", "Ring", "Slash");
	particleManager->CreateParticleGroup("magic1Group", "resources/images/gradationLine.png", "plane.obj", "Cylinder", "Magic1");
	particleManager->CreateParticleGroup("magic2Group", "resources/images/white.png", "plane.obj", "Triangle", "Magic2");
	particleManager->CreateParticleGroup("laserGroup", "resources/images/white.png", "plane.obj", "Cylinder", "Laser");
	particleManager->CreateParticleGroup("petalGroup", "resources/images/white.png", "plane.obj", "Petal", "Petal");

	particleManager->CreateParticleGroup("homingGroup", "resources/images/white.png", "plane.obj", "Ring", "Homing");
	particleManager->CreateParticleGroup("flameGroup", "resources/images/white.png", "plane.obj", "Ring", "Flame");

	particleManager->CreateParticleGroup("explosionGroup", "resources/images/white.png", "plane.obj", "Ring", "Explosion");
	particleManager->CreateParticleGroup("vignetteGroup", "resources/images/black.png", "plane.obj", "Ring", "Wiggle");
	particleManager->CreateParticleGroup("walk", "resources/images/white.png", "plane.obj", "Cube", "Dust");
	particleManager->CreateParticleGroup("enemyWalk", "resources/images/white.png", "plane.obj", "Cube", "EnemyDust");
	particleManager->CreateParticleGroup("debuff", "resources/images/white.png", "plane.obj", "Line", "Debuff");
	particleManager->CreateParticleGroup("spark", "resources/images/white.png", "plane.obj", "Triangle", "Spark");
	particleManager->CreateParticleGroup("sparkBurst", "resources/images/white.png", "plane.obj", "Triangle", "SparkBurst");
	particleManager->CreateParticleGroup("rupture", "resources/images/white.png", "plane.obj", "Triangle", "Rupture");
	particleManager->CreateParticleGroup("goal", "resources/images/white.png", "plane.obj", "Cube", "Fountain");
	particleManager->CreateParticleGroup("HitReaction", "resources/images/white.png", "plane.obj", "Cube", "HitReaction");
	particleManager->CreateParticleGroup("BltReaction", "resources/images/gradationLine.png", "plane.obj", "Ring", "BltReaction");

	// Cylinderを出すときに向き指定する
	ParticleMotion::SetDirection("UP");

	useExampleGroup_ = true;


	TextureManager::GetInstance()->LoadTexture("resources/images/studio.dds",true);
	uint32_t cubeSrvIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/images/studio.dds");
	skybox->SetCubeMapSrvIndex(cubeSrvIndex);
}

void MyGame::Finalize()
{
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}


	sceneFactory_.reset();

	Framework::Finalize();

}

void MyGame::Update()
{
	Framework::Update();

	// Skyboxの更新
	skybox->Update();

	if (time > 9.0f)
	{
		time = 0.0f;
	}

	time += 1.0f / 60.0f;


	if (Input::GetInstance()->TriggerKey(DIK_1))
	{
		particleManager->Emit("sparkBurst", { 0.0f,2.0f,0.0f }, 3);
	}

	if (Input::GetInstance()->TriggerKey(DIK_2))
	{
		particleManager->Emit("laserGroup", { 0.0f,1.0f,0.0f }, 3);
	}

	if (Input::GetInstance()->TriggerKey(DIK_3))
	{
		particleManager->Emit("petalGroup", { 0.0f,1.0f,0.0f }, 10);
		isPetal_ = !isPetal_;
	}

	if (Input::GetInstance()->TriggerKey(DIK_4))
	{
		particleManager->Emit("homingGroup", { 0.0f,1.0f,0.0f }, 3);
		isHoming_ = !isHoming_;
	}

	if (Input::GetInstance()->TriggerKey(DIK_5))
	{
		particleManager->Emit("flameGroup", { 0.0f,1.0f,0.0f }, 5);
		isFlame_ = !isFlame_;
	}

	if (Input::GetInstance()->TriggerKey(DIK_6))
	{
		particleManager->Emit("explosionGroup", { 0.0f,1.0f,0.0f }, 3);
		isExplosion_ = !isExplosion_;
	}

	if (std::fmod(time, 0.8f) < 0.1f && isPetal_)
	{
		particleManager->Emit("petalGroup", { 0.0f,1.0f,0.0f }, 8);
	}

	if (std::fmod(time, 1.0f) < 0.1f && isHoming_)
	{
		particleManager->Emit("homingGroup", { 0.0f,1.0f,0.0f }, 3);
	}

	if (std::fmod(time, 0.8f) < 0.1f && isFlame_)
	{
		particleManager->Emit("flameGroup", { 0.0f,1.0f,0.0f }, 5);
	}

	if (std::fmod(time, 1.0f) < 0.1f && isExplosion_)
	{
		particleManager->Emit("explosionGroup", { 0.0f,1.0f,0.0f }, 3);
	}




#ifdef USE_IMGUI

	//particleManager->DebugUI();

	if (ImGui::CollapsingHeader("Grayscale"))
	{
		static bool useGrayscale = false;
		if (ImGui::Checkbox("UseGrayscale", &useGrayscale))
		{
			postEffectManager->SetActiveEffect("Grayscale", useGrayscale);
		}
		if (ImGui::Button("1"))
		{
			if (useGrayscale_ == 1)
			{
				useGrayscale_ = 0;
			} 
			else 
			{
				useGrayscale_ = 1;
			}
		}
		if (ImGui::Button("2"))
		{
			if (useGrayscale_ == 2)
			{
				useGrayscale_ = 0;
			}
			else
			{
				useGrayscale_ = 2;
			}
		}

		// 現在値を表示
		ImGui::Text("useGrayscale_ = %u", useGrayscale_);

		// 値をGrayscalePassに反映
		postEffectManager->GetPassAs<GrayscalePass>("Grayscale")->SetUseGrayscale(useGrayscale_);

	}

	if (ImGui::CollapsingHeader("Vignette"))
	{

		static bool useVignette = false;
		if (ImGui::Checkbox("Use Vignette", &useVignette))
		{
			// ポストエフェクトマネージャに切り替えを伝える関数を作る想定
			postEffectManager->SetActiveEffect("Vignette",useVignette);
		}

		// VignetteのパラメータをImGuiで調整する
		if (ImGui::SliderFloat("Vignette Radius", &vignetteRadius_, 0.0f, 5.0f))
		{
			postEffectManager->GetPassAs<VignettePass>("Vignette")->SetStrength(vignetteRadius_);
		}
	}

	if (ImGui::CollapsingHeader("BoxFilter"))
	{
		static bool useBoxFilter = false;
		if (ImGui::Checkbox("Use BoxFilter", &useBoxFilter))
		{
			postEffectManager->SetActiveEffect("BoxFilter", useBoxFilter);
		}

		static float blurIntensity = 1.0f;
		ImGui::SliderFloat("Box Blur Intensity", &blurIntensity, 0.1f, 1.0f);
		postEffectManager->GetPassAs<BoxFilterPass>("BoxFilter")->SetIntensity(blurIntensity);
	}

	if (ImGui::CollapsingHeader("GaussianFilter"))
	{
		static bool useGaussianFilter = false;
		if (ImGui::Checkbox("Use GaussianFilter", &useGaussianFilter))
		{
			postEffectManager->SetActiveEffect("GaussianFilter", useGaussianFilter);
		}

		static float gaussianIntensity = 1.0f;
		ImGui::SliderFloat("Gaussian Blur Intensity", &gaussianIntensity, 0.1f, 5.0f);
		postEffectManager->GetPassAs<GaussianFilterPass>("GaussianFilter")->SetIntensity(gaussianIntensity);
	}

	// TimeManager 用の ImGui 操作パネルを追加
	if (ImGui::CollapsingHeader("TimeManager"))
	{
		// 現在のタイムスケールを取得
		float currentScale = TimeManager::Instance().GetTimeScale();

		// スライダーの表示値（ユーザー操作時に優先して反映）
		static float timeScaleSlider = currentScale;
		// 外部からタイムスケールが変更された場合、ユーザー操作中でなければ同期する
		if (!ImGui::IsAnyItemActive() && std::abs(timeScaleSlider - currentScale) > 1e-6f)
		{
			timeScaleSlider = currentScale;
		}

		// スライダーで直接変更
		if (ImGui::SliderFloat("Time Scale", &timeScaleSlider, 0.0f, 2.0f))
		{
			TimeManager::Instance().SetTimeScale(timeScaleSlider);
		}

		// 一時停止 / 再開 ボタン
		if (ImGui::Button("Pause"))
		{
			TimeManager::Instance().Pause();
		}
		ImGui::SameLine();
		if (ImGui::Button("Resume"))
		{
			TimeManager::Instance().Resume();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
		{
			TimeManager::Instance().SetTimeScale(1.0f);
			timeScaleSlider = 1.0f;
		}

		// スムーズ変更の入力
		static float smoothTarget = 0.0f;
		static float smoothDuration = 0.5f;
		ImGui::InputFloat("Smooth Target", &smoothTarget, 0.1f, 1.0f, "%.2f");
		ImGui::InputFloat("Smooth Duration (s)", &smoothDuration, 0.1f, 1.0f, "%.2f");

		if (ImGui::Button("Smooth To Target"))
		{
			// duration が負にならないように保護
			TimeManager::Instance().SmoothTimeScale(smoothTarget, std::max(0.0f, smoothDuration));
		}

		// ステップ実行
		static float stepSeconds = 0.016f;
		ImGui::InputFloat("Step Seconds", &stepSeconds, 0.001f, 0.1f, "%.3f");
		if (ImGui::Button("Step"))
		{
			TimeManager::Instance().Step(stepSeconds);
		}

		// 現在の値を表示
		ImGui::Text("Current TimeScale = %.3f", currentScale);
	}


#endif // USE_IMGUI

}

void MyGame::Draw()
{
	//ゲームの処理		描画処理

	//コマンドを積み込んで確定させる

	// ---------- オフスクリーン描画 ----------
	renderTexture->BeginRender();

	srvManager->PreDraw();

	// Skyboxの描画
	skybox->Draw();

	sceneManager_->Draw();   // 実際の描画

	particleManager->Draw();

	renderTexture->EndRender();


	// ---------- SwapChainへの描画 ----------
	dxCommon->PreDraw();
	
	postEffectManager->DrawAll(dxCommon->GetCommandList(), renderTexture->GetGPUHandle(), inputRes.Get(), state);

#ifdef USE_IMGUI
	// ImGui描画
	imGuiManager->Draw();
#endif // USE_IMGUI

	dxCommon->PostDraw();


}

void MyGame::LoadModel()
{
	ModelManager::GetInstance()->LoadModel("cube.obj");
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("sphere.obj");
	ModelManager::GetInstance()->LoadModel("terrain.obj");
	ModelManager::GetInstance()->LoadModel("player/player.obj");
	ModelManager::GetInstance()->LoadModel("player/bullet/playerBullet.obj");
	ModelManager::GetInstance()->LoadModel("enemy/normal/normalEnemy.obj");
	ModelManager::GetInstance()->LoadModel("enemy/normal/bullet/enemyBullet.obj");
	ModelManager::GetInstance()->LoadModel("enemy/trap/trapEnemy.obj");
	ModelManager::GetInstance()->LoadModel("enemy/trap/timeBomb/bomb.obj");
	ModelManager::GetInstance()->LoadModel("enemy/trap/vignette/vignette.obj");
	ModelManager::GetInstance()->LoadModel("wall/wall.obj");
	ModelManager::GetInstance()->LoadModel("field/field.obj");
	ModelManager::GetInstance()->LoadModel("goal/goal.obj");
	ModelManager::GetInstance()->LoadModel("logo/logo.obj");
	ModelManager::GetInstance()->LoadModel("barrie/barrie.obj");
	
}

void MyGame::LoadSound()
{
	soundData_ = Audio::GetInstance()->LoadWav("fanfare.wav");
	soundData2_ = Audio::GetInstance()->LoadWav("BGM.wav");
}