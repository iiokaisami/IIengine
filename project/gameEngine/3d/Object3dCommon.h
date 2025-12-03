#pragma once

#include <d3d12.h>
#include "../../externals/DirectXTex/d3dx12.h"

#include "DirectXCommon.h"
#include "Logger.h"
#include "CameraManager.h"

/// <summary>
/// 3Dオブジェクト共通機能
/// </summary>
class Object3dCommon
{
#pragma region シングルトンインスタンス

private:

	Object3dCommon() = default;
	~Object3dCommon() = default;
	Object3dCommon(Object3dCommon&) = delete;
	Object3dCommon& operator = (Object3dCommon&) = delete;

public:
	// シングルトンインスタンスの取得
	static Object3dCommon* GetInstance();
	// 終了
	void Finalize();

#pragma endregion シングルトンインスタンス

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon">DirectX共通機能</param>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 描画共通設定
	/// </summary>
	void CommonDrawSetting();

public: // セッター

	/// <summary>
	/// デフォルトカメラの設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetDefaultCamera(std::shared_ptr<Camera> camera) { defaultCamera_ = camera; }

public: // ゲッター

	/// <summary>
	/// DirectX共通機能の取得
	/// </summary>
	/// <returns>DirectX共通機能</returns>
	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	/// <summary>
	/// デフォルトカメラの取得
	/// </summary>
	/// <returns>カメラ</returns>
	std::shared_ptr<Camera> GetDefaultCamera() const { return defaultCamera_; }

private:

	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	void CreateGraphicsPipeline();

private:

	DirectXCommon* dxCommon_;

	std::shared_ptr<Camera> defaultCamera_ = nullptr;
	
	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	//ディスクリプタレンジの生成
	D3D12_DESCRIPTOR_RANGE descriptorRange_[2]{};
	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	//InputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc_{};
	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	//shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;
	

	//RootSignature作成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	//PSOを生成する
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;


	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_ROOT_PARAMETER rootParameters_[9] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};
};

