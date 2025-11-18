
#include "DirectXCommon.h"

#include <cassert>
#include <thread>
#include <d3d12.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#pragma comment(lib,"winmm.lib")

using namespace Microsoft::WRL;

const uint32_t DirectXCommon::kMaxSRVCount = 512;

void DirectXCommon::Initialize(WinApp* winApp)
{
	// NULL検出
	assert(winApp);
	// 借りてきたWinAppのインスタンスを記録
	this->winApp_ = winApp;

	// FPS固定初期化
	InitializeFixFPS();
	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);


	// デバイスの初期化
	InitializeDevice();

	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();



	// ディスクリプタヒープの生成
	CreateDescriptorHeap();

	// 深度バッファの生成
	CreateDepthBuffer();




	// レンダーターゲットビューの初期化
	InitializeFinalRenderTargets();

	// 深度ステンシルビューの初期化
	InitializeDepthStencilView();

	// フェンス生成
	CreateFence();

	// ビューポート矩形の初期化
	InitializeViewPort();

	// シザリング矩形の設定
	InitializeScissor();

	// DXCコンパイラの生成
	CreateDXCompiler();

	// ImGuiの初期化
	//InitializeImGui(); 9章でやる

	//FenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);


}

void DirectXCommon::ReportLiveObjects()
{
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	}
}

void DirectXCommon::Finalize()
{
	// 既存のリソース解放処理

   // Report live objects
	ReportLiveObjects();
}

void DirectXCommon::InitializeDevice()
{
	HRESULT result = S_FALSE;

	//デバッグレイヤー
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif


	//DXGIファクトリーの生成

	//HRESULTはWindows刑のエラーコードであり、
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	//どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(result));

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//いい順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
	{
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		result = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(result));	//取得できないのは一大事
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//採用したアダプタの情報をログに出力。wstringの方なので注意
			Logger::Log(StringUtility::ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);



	//機能レベルとログの出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(result))
		{
			//生成できたのでログ出力を行ってループを抜ける
			Logger::Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Logger::Log("Complete create D3D12Device!!!");//初期化完了のログを出す



#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//windows11までのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		//解放
		//infoQueue->Release();
	}
#endif
}

void DirectXCommon::InitializeCommand()
{
	HRESULT result = S_FALSE;

	// コマンドアロケータを生成
	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));

	// コマンドリストを生成
	result = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));

	// 標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));
}

void DirectXCommon::CreateSwapChain()
{
	HRESULT result = S_FALSE;

	//スワップチェインを生成する
	swapChainDesc_.Width = WinApp::kClientWidth;						// 画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = WinApp::kClientHeight;						// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Format = /*DXGI_FORMAT_R8G8B8A8_UNORM_SRGB*/DXGI_FORMAT_R8G8B8A8_UNORM;				    // 色の形式
	swapChainDesc_.SampleDesc.Count = 1;								// マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	    // 描画のターゲットとして利用する
	swapChainDesc_.BufferCount = 2;									    // ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;		    // モニタにうつしたら、中身を廃棄

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	result = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(result));

}

void DirectXCommon::CreateDepthBuffer()
{
	HRESULT result = S_FALSE;

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;						//VRAM上に作る

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = WinApp::kClientWidth;											//Textureの幅
	resourceDesc.Height = WinApp::kClientHeight;										//Textureの高さ
	resourceDesc.MipLevels = 1;											//mipmapの数
	resourceDesc.DepthOrArraySize = 1;									//奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;									//サンプリングカウント。１個指定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;		//２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;		//DepthStencilとして使う通知

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;							//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;				//フォーマット。Resourceと合わせる

	//resourceの生成
	result = device_->CreateCommittedResource(
		&heapProperties,												//Heapの設定
		D3D12_HEAP_FLAG_NONE,											//Heapの特殊な設定。特になし
		&resourceDesc,													//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,								    //深度値を書き込む状態にしておく
		&depthClearValue,		//Clear最適値
		IID_PPV_ARGS(&resource_)											//作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(result));


	InitializeDepthStencilView();
}

void DirectXCommon::CreateDescriptorHeap()
{
	//RTV用のヒープでディスクリプタの数は２。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//SRV用のヒープでディスクリプタの数は１２８。SRVはShader内で触るものなので、ShaderVisibleはtrue
	srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxSRVCount, true);

	//DSV用のヒープでディスクリプタの数は１。DSVはSharder内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);


	//ディスクリプタヒープのサイズ
	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptor, bool shaderVisible)
{
	HRESULT result = S_FALSE;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptor;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(result));

	return descriptorHeap;
}

void DirectXCommon::InitializeFinalRenderTargets()
{
	HRESULT result = S_FALSE;


	//SwapChainからResourceを引っ張ってくる
	result = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	//うまく取得できなければ起動できない
	assert(SUCCEEDED(result));
	result = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(result));


	//RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	// 裏表の2つ分
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	rtvHandles_[0] = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, 0);

	//２つ目_のディスクリプタハンドルを得る
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//rtvHandles_[1] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);
	rtvHandles_[1] = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, 1);


	//描画先のRTVとDSVを設定する
	dsvHandle_ = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVCPUDescriptorHandle(uint32_t index)
{
	return GetCPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVGPUDescriptorHandle(uint32_t index)
{
	return GetGPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV_, index);
}

void DirectXCommon::InitializeDepthStencilView()
{
	//DepthStencilTextureをウィンドウのサイズで作成
	//Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResource(device, WinApp::kClientWidth, WinApp::kClientHeight);
	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(
		resource_.Get(),
		&dsvDesc,
		dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart()
	);
}

void DirectXCommon::CreateFence()
{
	HRESULT result = S_FALSE;

	// フェンスの生成
	//初期値0でFenceを作る
	result = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(result));
}

void DirectXCommon::InitializeViewPort()
{
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = WinApp::kClientWidth;
	viewport_.Height = WinApp::kClientHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void DirectXCommon::InitializeScissor()
{
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight;
}

void DirectXCommon::CreateDXCompiler()
{
	HRESULT result = S_FALSE;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

void DirectXCommon::InitializeImGui()
{
#ifdef USE_IMGUI

	//ImGuiの初期化
	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	ImGui_ImplDX12_Init(
		device_.Get(),
		swapChainDesc_.BufferCount,
		rtvDesc_.Format,
		srvDescriptorHeap_.Get(),
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
	);*/

#endif // USE_IMGUI
}

void DirectXCommon::PreDraw()
{
	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();


	//TransitionBarrierの設定
	//今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対称のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	//遷移前（現在）のresourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//transitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);




	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle_);


	//指定した色で画面全体をクリアする
	float clearColor[] = { 1.0f,0.98f,0.85f,1.0f };//{ 0.1f,0.25f,0.5f,1.0f };	//青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(
		dsvHandle_,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr
	);

	//描画用のDescriptorHeapの設定
	//もろもろの描画処理が終わったタイミングでImGuiの描画コマンドを積む
	//Guiは画面の最前面に映すので、一番最後の描画として行う
	//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { srvDescriptorHeap_ };
	//commandList_->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());

	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);
}

void DirectXCommon::PostDraw()
{
	HRESULT result = S_FALSE;

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//画面の各処理はすべて終わり、画面に移すので、状態を遷移
	//今回はRenderTargetからPresentにする
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier_.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier_.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;


	//TransitonのBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);



	//コマンドリストの内容を確定させる。すべてのコマンドを頼んでからCloseすること
	result = commandList_->Close();
	assert(SUCCEEDED(result));


	// FPS固定
	UpdateFixFPS();


	//コマンドをキックする
	//GPU二コマンドリストの実行を行わせる
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	//GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);



	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値を代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompleteValueの初期値はFence制作時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_)
	{
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//次フレーム用のコマンドリストを準備
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}

Microsoft::WRL::ComPtr<IDxcBlob> DirectXCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile)
{
	HRESULT result = S_FALSE;

	//hlslファイルを読む

	//これからシェーダーをコンパイルする旨をログに出す
	Logger::Log(StringUtility::ConvertString(std::format(L"Begin CompileSharder, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	result = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(result));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;		//UTF8の文字コードであることを通知

	//コンパイルする

	LPCWSTR arguments[] = {
		filePath.c_str(),				//コンパイル対称のhlslファイル名
		L"-E", L"main",					//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile,					//Sharderprofileの設定
		L"-Zi", L"-Qembed_debug",		//デバッグ用の情報を埋め込む
		L"-Od",							//最適化を外しておく
		L"-Zpr",						//メモリレイアウトは行優先
	};

	//実際にシェーダーをコンパイルする
	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_,
		IID_PPV_ARGS(&shaderResult)
	);
	//コンパイルエラーではなくdxcで起動できないなど致命的な状況
	assert(SUCCEEDED(result));



	//警告・エラーが出ていないか確認する


	//警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0)
	{
		Logger::Log(shaderError->GetStringPointer());
		//警告・エラーダメ絶対
		assert(SUCCEEDED(false));
	}



	//コンパイル結果を受け取って返す

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//成功したログを出す
	Logger::Log(StringUtility::ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateBufferResource(size_t sizeInBytes)
{
	HRESULT result = S_FALSE;

	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource = nullptr;
	//頂点リソース用のヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	//頂点リソースの設定
	D3D12_RESOURCE_DESC bufferResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferResourceDesc.Width = (sizeInBytes + 255) & ~255;
	//バッファの場合はこれらは１にする決まり
	bufferResourceDesc.Height = 1;
	bufferResourceDesc.DepthOrArraySize = 1;
	bufferResourceDesc.MipLevels = 1;
	bufferResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	result = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&bufferResource));
	assert(SUCCEEDED(result));
	result;
	return bufferResource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
	HRESULT result = S_FALSE;

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);									//Textureの幅
	resourceDesc.Height = UINT(metadata.height);								//Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);						//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);					//奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;										//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;											//サンプリングカウント。１固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);		//Textureの次元数。普段使っているのは２次元

	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;								//細かい設定を行う

	//resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	result = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(result));


	return resource;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device_.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);
	UpdateSubresources(commandList_.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERI_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList_->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

DirectX::ScratchImage DirectXCommon::LoadTexture(const std::string& filePath)
{
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds"))
	{
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else
	{
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
	}
	
	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format))
	{
		//圧縮テクスチャの場合は、ミップマップを生成しない
		mipImages = std::move(image);
	} 
	else
	{
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
		assert(SUCCEEDED(hr));
	}
	
	//ミップマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(int32_t width, int32_t height)
{
	HRESULT hr = S_FALSE;

	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;                                   // Textureの幅　
	resourceDesc.Height = height;                                 // Textureの高さ
	resourceDesc.MipLevels = 1;                                   // mipMapの数
	resourceDesc.DepthOrArraySize = 1;                            // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;          // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;                            // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;  // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;              // 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる

	// Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	hr = device_->CreateCommittedResource(
		&heapProperties,                  // Heapの設定
		D3D12_HEAP_FLAG_NONE,             // Heapの特殊な設定
		&resourceDesc,                    // Resource設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 初回のResourceState。Textureは読むだけ
		&depthClearValue,                 // Clear最適値
		IID_PPV_ARGS(&resource)           // 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}

void DirectXCommon::CommandPass()
{
	HRESULT result = S_FALSE;

	//コマンドリストの内容を確定させる。すべてのコマンドを頼んでからCloseすること
	result = commandList_->Close();
	assert(SUCCEEDED(result));

	//コマンドをキックする
	//GPU二コマンドリストの実行を行わせる
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
	//GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);



	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値を代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompleteValueの初期値はFence制作時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_)
	{
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//次フレーム用のコマンドリストを準備
	result = commandAllocator_->Reset();
	assert(SUCCEEDED(result));
	result = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
}

void DirectXCommon::CreateSamplerHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HRESULT hr = device_->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&samplerHeap_));
	assert(SUCCEEDED(hr) && "Failed to create Sampler Heap!");
	hr;

	// デバッグログ
	D3D12_GPU_DESCRIPTOR_HANDLE samplerHeapBase = samplerHeap_->GetGPUDescriptorHandleForHeapStart();
	OutputDebugStringA(("Sampler Heap Base Address: " + std::to_string(samplerHeapBase.ptr) + "\n").c_str());
	assert(samplerHeap_ != nullptr && "Sampler Descriptor Heap is null!");

	D3D12_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	device_->CreateSampler(&samplerDesc, samplerHeap_->GetCPUDescriptorHandleForHeapStart());
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateUploadBuffer(size_t sizeInBytes)
{
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = sizeInBytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer));

	if (FAILED(hr)) {
		Logger::Log("Failed to create upload buffer!");
		return nullptr;
	}

	return buffer;
}

void DirectXCommon::InitializeFixFPS()
{
	// 現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DirectXCommon::UpdateFixFPS()
{
	// 1/60秒ピッタリの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回時間からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒（よりわずかに短い時間）経っていない場合
	if (elapsed < kMinCheckTime)
	{
		// 1/60経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime)
		{
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	// 現在の時間を記録
	reference_ = std::chrono::steady_clock::now();
}
