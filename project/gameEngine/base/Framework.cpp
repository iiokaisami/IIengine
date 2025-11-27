#include "Framework.h"

void Framework::Run()
{
	Initialize();

	while (true)
	{
		// 終了リクエストが来たら抜ける
		if (IsEndRequest())
		{
			break;
		}

		// 時間管理
		TimeManager::Instance().Tick();

#ifdef USE_IMGUI
		// ImGui開始
		imGuiManager->Begin();

#endif // USE_IMGUI

		// 毎フレーム更新
		Update();

#ifdef USE_IMGUI
		// ImGui終了
		imGuiManager->End();
#endif // USE_IMGUI

		// 描画
		Draw();
	}

	// ゲームの終了
	Finalize();

}

void Framework::Initialize()
{
	// WindowsAPIの初期化
	winApp = std::make_unique<WinApp>();
	winApp->Initialize();

	// DirectXの初期化
	dxCommon = std::make_unique<DirectXCommon>();
	dxCommon->Initialize(winApp.get());

	// SRVマネージャーの初期化
	srvManager = std::make_unique<SrvManager>();
	srvManager->Initialize(dxCommon.get());

	modelCommon = std::make_unique<ModelCommon>();
	modelCommon->Initialize(dxCommon.get());

	// キーボード入力
	input = Input::GetInstance();
	input->Initialize(winApp.get());

	// オーディオ
	audio = Audio::GetInstance();
	audio->Initialize();

	// シーンマネージャ
	sceneManager_ = SceneManager::GetInstance();

	// スプライト共通部分の初期化
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon.get());

	// テクスチャマネージャー
	textureManager = TextureManager::GetInstance();
	textureManager->Initialize(dxCommon.get(), srvManager.get());

	// 3Dオブジェクト
	object3dCommon = Object3dCommon::GetInstance();
	object3dCommon->Initialize(dxCommon.get());

	// モデル共通部分の初期化
	modelManager = ModelManager::GetInstance();
	modelManager->Initialize(dxCommon.get());

	// レンダーテクスチャ
	renderTexture = std::make_unique<RenderTexture>();
	renderTexture->Initialize(dxCommon.get(), srvManager.get(), WinApp::kClientWidth, WinApp::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Vector4(0.1f, 0.25f, 0.5f, 1.0f));

	// ポストエフェクト
	noneEffectPass = std::make_unique<NoneEffectPass>();
	noneEffectPass->Initialize(dxCommon.get(), srvManager.get(), L"resources/shaders/NoneEffect.VS.hlsl", L"resources/shaders/NoneEffect.PS.hlsl");
	grayscalePass = std::make_unique<GrayscalePass>();
	grayscalePass->Initialize(dxCommon.get(), srvManager.get(), L"resources/shaders/Grayscale.VS.hlsl", L"resources/shaders/Grayscale.PS.hlsl");
	vignettePass = std::make_unique<VignettePass>();
	vignettePass->Initialize(dxCommon.get(), srvManager.get(), L"resources/shaders/Vignette.VS.hlsl", L"resources/shaders/Vignette.PS.hlsl");
	boxFilterPass = std::make_unique<BoxFilterPass>();
	boxFilterPass->Initialize(dxCommon.get(), srvManager.get(), L"resources/shaders/BoxFilter.VS.hlsl", L"resources/shaders/BoxFilter.PS.hlsl");
	gaussianFilterPass = std::make_unique<GaussianFilterPass>();
	gaussianFilterPass->Initialize(dxCommon.get(), srvManager.get(), L"resources/shaders/GaussianFilter.VS.hlsl", L"resources/shaders/GaussianFilter.PS.hlsl");

	postEffectManager = PostEffectManager::GetInstance();
	postEffectManager->SetNoneEffect(std::move(noneEffectPass));
	postEffectManager->AddPass("Grayscale", std::move(grayscalePass));
	postEffectManager->AddPass("Vignette", std::move(vignettePass));
	postEffectManager->AddPass("BoxFilter", std::move(boxFilterPass));
	postEffectManager->AddPass("GaussianFilter", std::move(gaussianFilterPass));
	

	// パーティクル	
	particleManager = ParticleManager::GetInstance();
	particleManager->Initialize(dxCommon.get(),srvManager.get(),modelCommon.get());

	inputSrv = renderTexture->GetSRVHandle();
	inputRes = renderTexture->GetResource();
	state = renderTexture->GetCurrentState();

	// Skybox
	skybox = std::make_unique<Skybox>();
	skybox->Initialize(dxCommon.get(), srvManager.get());

#ifdef USE_IMGUI

	imGuiManager = std::make_unique<ImGuiManager>();
	imGuiManager->Initialize(winApp.get(), dxCommon.get());

#endif // USE_IMGUI
		
}

void Framework::Finalize()
{
	// WindowsAPIの終了処理
	winApp->Finalize();
	// WindowsAPI解放
	winApp.reset();
	winApp = nullptr;

	// DirectX解放
	dxCommon.reset();

	// SRVマネージャー解放
	srvManager.reset();

	sceneManager_->Finalize();
	delete sceneFactory_;

	input->Finalize();

	audio->Finalize();

	// スプライト共通部分解放
	spriteCommon->Finalize();

	textureManager->Finalize();

	object3dCommon->Finalize();

	modelManager->Finalize();

	// レンダーテクスチャ解放
	renderTexture.reset();
	renderTexture = nullptr;
	


	particleManager->Finalize();

	// skybox解放
	skybox->Finalize();

#ifdef USE_IMGUI
	// ImGuiManager解放
	imGuiManager->Finalize();
	imGuiManager.reset();
#endif // USE_IMGUI
}

void Framework::Update()
{
	// シーンマネージャーの更新
	sceneManager_->Update();
	
	// 入力更新
	input->Update();

	// パーティクル更新
	particleManager->Update();
}
