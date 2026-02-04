#pragma once

#include "BasePostEffectPass.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <string>

namespace IIEngine
{

	/// <summary>
	/// ボックスフィルタ
	/// 適度にぼかすことでジャギーを目立たなくする
	/// </summary>
	class BoxFilterPass : public BasePostEffectPass
	{
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="dxCommon">DirectX共通機能管理クラスへのポインタ</param>
		/// <param name="srvManager">SRV管理クラスへのポインタ</param>
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
		/// <returns></returns>
		std::string GetName() const override { return "BoxFilter"; }

		/// <summary>
		/// 定数バッファの更新
		/// </summary>
		void UpdateConstantBuffer();

	public: // セッター

		/// <summary>
		/// ぼかし強度設定
		/// </summary>
		/// <param name="intensity">ぼかしの強度(0.0f以上の値)</param>
		void SetIntensity(float intensity) { cbData_.intensity = intensity; }

	private:

		struct BoxFilterCB {
			float intensity;
			float padding[3];
		};

		BoxFilterCB cbData_ = {};
		// GPU上の定数バッファリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
		// マップされたCPUポインタ（cbData_を書き込む先）
		BoxFilterCB* mappedCB_ = nullptr;

	};

}