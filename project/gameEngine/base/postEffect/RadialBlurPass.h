#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <string>

#include "BasePostEffectPass.h"

#include <MyMath.h>

namespace IIEngine
{
	/// <summary>
	/// 放射状ブラー
	/// 中心から放射状にぼかすエフェクト
	/// </summary>
	class RadialBlurPass : public BasePostEffectPass
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
		std::string GetName() const override { return "RadialBlur"; }

		/// <summary>
		/// 定数バッファ更新
		/// </summary>
		void UpdateConstantBuffer();

	public: // セッター

		/// <summary>
		/// 中心設定
		/// </summary>
		/// <param name="c">中心座標</param>
		void SetCenter(const Vector2& c) { cbData_.center = c; }
		/// <summary>
		/// 強度設定
		/// </summary>
		/// <param name="s">強度</param>
		void SetStrength(float s) { cbData_.strength = s; }
		/// <summary>
		/// サンプル数設定
		/// </summary>
		/// <param name="count">サンプル数</param>
		void SetSampleCount(int count) { cbData_.sampleCount = count; }

	private:

		// 定数バッファ用データ構造体
		struct RadialBlurCB
		{
			Vector2 center;
			float strength;
			int sampleCount;
			float padding[2];
		};

		// 定数バッファの初期値
		RadialBlurCB cbData_ =
		{
			{0.5f, 0.5f},
			0.0f,
			8
		};

		// 定数バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
		// 定数バッファにマップしたポインタ
		RadialBlurCB* mappedCB_ = nullptr;

	};
}