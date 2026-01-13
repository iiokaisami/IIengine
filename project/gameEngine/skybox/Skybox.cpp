#include "Skybox.h"

#include <DirectXCommon.h>
#include <SrvManager.h>

#include "../3d/CameraManager.h"

namespace IIEngine
{

	void Skybox::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
	{
		dxCommon_ = dxCommon;
		srvManager_ = srvManager;

		// 頂点・インデックスバッファの生成
		CreateBuffers();
		// 定数バッファの生成
		CreateConstantBuffer();
		// ルートシグネチャの生成
		CreateRootSignature();
		// パイプラインステートの生成
		CreatePipelineState();
	}

	void Skybox::Finalize()
	{
		vertexBuffer_.Reset();
		indexBuffer_.Reset();
		constantBuffer_.Reset();
		rootSignature_.Reset();
		pipelineState_.Reset();
		dxCommon_ = nullptr;
		cubeMapSrvIndex_ = 0;
	}

	void Skybox::Update()
	{
		// カメラからビュー行列・射影行列を取得
		Matrix4x4 view = CameraManager::GetInstance().GetActiveCamera()->GetViewMatrix();//dxCommon_->GetCamera()->GetViewMatrix();
		Matrix4x4 proj = CameraManager::GetInstance().GetActiveCamera()->GetProjectionMatrix();

		// カメラの位置成分をゼロに（Skyboxはカメラに追従させる）
		view.m[0][3] = 0.0f;
		view.m[1][3] = 0.0f;
		view.m[2][3] = 0.0f;

		// Z反転行列（左手座標系で奥を向くCubeMap用）
		Matrix4x4 flipZ = {
			1, 0,  0, 0,
			0, 1,  0, 0,
			0, 0, -1, 0,
			0, 0,  0, 1
		};

		// スケーリング（Skyboxを十分大きくする）
		Matrix4x4 scale = MakeScaleMatrix({ 1000.0f, 1000.0f, 1000.0f });

		Matrix4x4 wvp = scale * view * proj * flipZ;
		memcpy(&mappedConstBuffer_->viewProj, &wvp, sizeof(Matrix4x4));
	}

	void Skybox::Draw()
	{
		ID3D12GraphicsCommandList* cmdList = dxCommon_->GetCommandList().Get();

		// パイプラインとルートシグネチャ設定
		cmdList->SetGraphicsRootSignature(rootSignature_.Get());
		cmdList->SetPipelineState(pipelineState_.Get());

		// ルートパラメータ  定数バッファ b0（VS用）
		cmdList->SetGraphicsRootConstantBufferView(0, constantBuffer_->GetGPUVirtualAddress());
		cmdList->SetGraphicsRootConstantBufferView(1, materialBuffer_->GetGPUVirtualAddress());

		// ルートパラメータ  SRV (CubeMap) をピクセルシェーダーに渡す（Descriptor Table）
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvManager_->GetGPUDescriptorHandle(cubeMapSrvIndex_);
		cmdList->SetGraphicsRootDescriptorTable(2, gpuHandle);

		// 頂点・インデックスバッファ設定
		cmdList->IASetVertexBuffers(0, 1, &vbView_);
		cmdList->IASetIndexBuffer(&ibView_);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 描画呼び出し
		cmdList->DrawIndexedInstanced(static_cast<UINT>(indexData_.size()), 1, 0, 0, 0);
	}

	void Skybox::CreateBuffers()
	{
		// 右面
		vertexData_[0].position = { 1.0f,1.0f,1.0f };
		vertexData_[1].position = { 1.0f,1.0f,-1.0f };
		vertexData_[2].position = { 1.0f,-1.0f,1.0f };
		vertexData_[3].position = { 1.0f,-1.0f,-1.0f };
		// 左面
		vertexData_[4].position = { -1.0f,1.0f,-1.0f };
		vertexData_[5].position = { -1.0f,1.0f,1.0f };
		vertexData_[6].position = { -1.0f,-1.0f,-1.0f };
		vertexData_[7].position = { -1.0f,-1.0f,1.0f };
		// 前面
		vertexData_[8].position = { -1.0f,1.0f,1.0f };
		vertexData_[9].position = { 1.0f,1.0f,1.0f };
		vertexData_[10].position = { -1.0f,-1.0f,1.0f };
		vertexData_[11].position = { 1.0f,-1.0f,1.0f };
		// 後面
		vertexData_[12].position = { 1.0f,1.0f,-1.0f };
		vertexData_[13].position = { -1.0f,1.0f,-1.0f };
		vertexData_[14].position = { 1.0f,-1.0f,-1.0f };
		vertexData_[15].position = { -1.0f,-1.0f,-1.0f };
		// 上面
		vertexData_[16].position = { -1.0f,1.0f,1.0f };
		vertexData_[17].position = { 1.0f,1.0f,1.0f };
		vertexData_[18].position = { -1.0f,1.0f,-1.0f };
		vertexData_[19].position = { 1.0f,1.0f,-1.0f };
		// 下面
		vertexData_[20].position = { -1.0f,-1.0f,-1.0f };
		vertexData_[21].position = { 1.0f,-1.0f,-1.0f };
		vertexData_[22].position = { -1.0f,-1.0f,1.0f };
		vertexData_[23].position = { 1.0f,-1.0f,1.0f };

		for (int i = 0; i < vertexData_.size(); ++i)
		{
			vertexData_[i].texcoord = vertexData_[i].position;
		}

		// 頂点バッファ作成
		size_t vertexBufferSize = sizeof(vertexData_);
		vertexBuffer_ = dxCommon_->CreateBufferResource(vertexBufferSize);

		// GPUメモリに転送
		SkyboxVertex* mappedVertex = nullptr;
		vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertex));
		memcpy(mappedVertex, vertexData_.data(), vertexBufferSize);
		vertexBuffer_->Unmap(0, nullptr);

		vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
		vbView_.SizeInBytes = static_cast<UINT>(vertexBufferSize);
		vbView_.StrideInBytes = sizeof(SkyboxVertex);

		// インデックスバッファ作成
		indexData_ =
		{
			// 右面
			0, 2, 1, 1, 2, 3,
			// 左面
			4, 6, 5, 5, 6, 7,
			// 前面
			8, 10, 9, 9, 10, 11,
			// 後面
			12, 14, 13, 13, 14, 15,
			// 上面
			16, 18, 17, 17, 18, 19,
			// 下面
			20, 22, 21, 21, 22, 23
		};
		size_t indexBufferSize = sizeof(indexData_);
		indexBuffer_ = dxCommon_->CreateBufferResource(indexBufferSize);

		uint16_t* mappedIndex = nullptr;
		indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
		memcpy(mappedIndex, indexData_.data(), indexBufferSize);
		indexBuffer_->Unmap(0, nullptr);

		ibView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
		ibView_.SizeInBytes = static_cast<UINT>(indexBufferSize);
		ibView_.Format = DXGI_FORMAT_R16_UINT;
	}

	void Skybox::CreateConstantBuffer()
	{
		// 定数バッファの作成（ViewProjection行列）
		constantBuffer_ = dxCommon_->CreateBufferResource(sizeof(ConstBufferData));
		constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedConstBuffer_));

		materialBuffer_ = dxCommon_->CreateBufferResource(sizeof(Material));
		materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedMaterial_));
		mappedMaterial_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMStoreFloat4x4(&mappedMaterial_->uvTransform, DirectX::XMMatrixIdentity());
	}

	void Skybox::CreateRootSignature()
	{
		D3D12_ROOT_PARAMETER rootParams[3]{};

		// b0: VS用 定数バッファ（viewProj）
		rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParams[0].Descriptor.ShaderRegister = 0;
		rootParams[0].Descriptor.RegisterSpace = 0;

		// b1: PS用 Material定数バッファ（color, uvTransform）
		rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[1].Descriptor.ShaderRegister = 1;
		rootParams[1].Descriptor.RegisterSpace = 0;

		// t0 (TextureCube)
		D3D12_DESCRIPTOR_RANGE descRange{};
		descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descRange.NumDescriptors = 1;
		descRange.BaseShaderRegister = 0;
		descRange.RegisterSpace = 0;
		descRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParams[2].DescriptorTable.NumDescriptorRanges = 1;
		rootParams[2].DescriptorTable.pDescriptorRanges = &descRange;

		// Sampler
		D3D12_STATIC_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc.ShaderRegister = 0;
		samplerDesc.RegisterSpace = 0;
		samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// シリアライズと作成
		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.NumParameters = _countof(rootParams);
		desc.pParameters = rootParams;
		desc.NumStaticSamplers = 1;
		desc.pStaticSamplers = &samplerDesc;
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1,
			&signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			assert(false);
		}

		dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
			signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature_));
	}

	void Skybox::CreatePipelineState()
	{
		// シェーダ読み込み
		auto vsBlob = dxCommon_->CompileShader(L"resources/shaders/Skybox.VS.hlsl", L"vs_6_0");
		auto psBlob = dxCommon_->CompileShader(L"resources/shaders/Skybox.PS.hlsl", L"ps_6_0");

		// 入力レイアウト
		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
		desc.InputLayout = { inputLayout, _countof(inputLayout) };
		desc.pRootSignature = rootSignature_.Get();
		desc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
		desc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };

		D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		desc.RasterizerState = rasterizerDesc;

		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		D3D12_DEPTH_STENCIL_DESC depthDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthDesc.StencilEnable = FALSE;
		desc.DepthStencilState = depthDesc;

		desc.SampleMask = UINT_MAX; // 全サンプルを有効化
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;

		HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState_));
		assert(SUCCEEDED(hr));
		hr;
	}

}