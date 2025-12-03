
#pragma once
#include <d3d12.h>
#include "../../externals/DirectXTex/d3dx12.h"

#include "DirectXCommon.h"
#include "Logger.h"

/// <summary>
/// スプライト共通クラス
/// </summary>
class SpriteCommon
{
#pragma region シングルトンインスタンス
private:

	SpriteCommon() = default;
	~SpriteCommon() = default;
	SpriteCommon(SpriteCommon&) = delete;
	SpriteCommon& operator = (SpriteCommon&) = delete;

public:
	// シングルトンインスタンスの取得
	static SpriteCommon* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon">DirectXCommonのポインタ</param>
	void Initialize(DirectXCommon* dxCommon);

	// ルートシグネイチャの作成
	void CreateRootSignature();

	// グラフィックスパイプラインの生成
	void CreateGraphicsPipeline();

	// 共通描画設定
	void CommonDrawSetting();

public: // ゲッター

	/// <summary>
	/// DirectXCommonのポインタを取得
	/// </summary>
	/// <returns>DirectXCommonのポインタ</returns>
	DirectXCommon* GetDxCommon() const { return dxCommon_; }

private:

	DirectXCommon* dxCommon_;

	// デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	//ディスクリプタレンジの生成
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1]{};
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

};
