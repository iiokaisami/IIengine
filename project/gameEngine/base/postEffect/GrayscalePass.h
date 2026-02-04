#pragma once

#include "BasePostEffectPass.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <string>

namespace IIEngine
{

	/// <summary>
	/// グレースケールエフェクト
	/// グレースケール化するかどうかを切り替え可能
	/// </summary>
	class GrayscalePass : public BasePostEffectPass
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
		/// パス名の取得
		/// </summary>
		/// <returns>パス名</returns>
		std::string GetName() const override { return "Grayscale"; }

		/// <summary>
		/// 定数バッファの更新
		/// </summary>
		void UpdateConstantBuffer();

	public: // セッター

		/// <summary>
		/// グレースケール使用設定
		/// </summary>
		/// <param name="enable">有効化フラグ</param>
		void SetUseGrayscale(uint32_t enable) { cbData_.useGrayscale = enable; }

	private:
		struct GrayscaleCB {
			uint32_t useGrayscale;
			float padding[3];
		};

		GrayscaleCB cbData_ = {};

		// GPU上の定数バッファリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
		// マップされたCPUポインタ（cbData_を書き込む先）
		GrayscaleCB* mappedCB_ = nullptr;

	};
}