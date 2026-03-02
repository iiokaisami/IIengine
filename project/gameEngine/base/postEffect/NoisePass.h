#pragma once

#include <wrl.h>

#include "BasePostEffectPass.h"

#include <MyMath.h>

namespace IIEngine
{
	class NoisePass : public BasePostEffectPass
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
		std::string GetName() const override { return "Noise"; }

		/// <summary>
		/// 定数バッファ更新
		/// </summary>
		void UpdateConstantBuffer();

	public: // セッター

		/// <summary>
		/// ノイズの強さを設定
		/// </summary>
		/// <param name="i">強さ</param>
		void SetIntensity(float i) { cbData_.intensity = i; }

		/// <summary>
		/// 時間を設定
		/// </summary>
		/// <param name="t">時間</param>
		void SetTime(float t) { cbData_.time = t; }

	private:

		struct NoiseCB
		{
			float intensity;
			float time;
			Vector2 padding;
		};

		NoiseCB cbData_;
		NoiseCB* mappedCB_ = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;

	};

}