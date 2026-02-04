#pragma once

#include "BasePostEffectPass.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <string>

namespace IIEngine
{

	/// <summary>
	/// ガウシアンフィルタ
	/// 画面をぼかすエフェクト
	/// </summary>
	class GaussianFilterPass : public BasePostEffectPass
	{
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="dxCommon">DirectX共通機能管理クラスへのポインタ</param>
		/// <param name="srvManager">シェーダリソースビュー管理クラスへのポインタ</param>
		/// <param name="vsPath">頂点シェーダファイルのパス</param>
		/// <param name="psPath">ピクセルシェーダファイルのパス</param>
		void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath) override;

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="cmdList">描画コマンドリスト</param>
		/// <param name="inputSrvHandle">入力テクスチャのSRVハンドル</param>
		/// <param name="inputResource">入力リソース</param>
		/// <param name="currentState">入力リソースの現在の状態</param>
		void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrvHandle, ID3D12Resource* inputResource, D3D12_RESOURCE_STATES& currentState) override;

		/// <summary>
		/// パス名取得
		/// </summary>
		/// <returns>パス名</returns>
		std::string GetName() const override { return "GaussianFilter"; }

		/// <summary>
		/// 定数バッファ更新
		/// </summary>
		void UpdateConstantBuffer();

	public: // セッター

		/// <summary>
		/// 強度設定
		/// </summary>
		/// <param name="intensity">強度</param>
		void SetIntensity(float intensity) { cbData_.intensity = intensity; }

	private:

		struct GaussianFilterCB {
			float intensity;
			float padding[3];
		};

		GaussianFilterCB cbData_ = {};
		// GPU上の定数バッファリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
		// マップされたCPUポインタ（cbData_を書き込む先）
		GaussianFilterCB* mappedCB_ = nullptr;

	};
}
