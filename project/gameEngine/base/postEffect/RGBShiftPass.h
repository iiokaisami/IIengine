#pragma once

#include <wrl.h>

#include "BasePostEffectPass.h"

#include <MyMath.h>

namespace IIEngine
{

	class RGBShiftPass : public BasePostEffectPass
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
		std::string GetName() const override { return "RGBShift"; }

		/// <summary>
		/// 定数バッファ更新
		/// </summary>
		void UpdateConstantBuffer();

	public: // セッター

		/// <summary>
		/// シフトの強さを設定
		/// </summary>
		/// <param name="intensity">強さ</param>
		void SetIntensity(float intensity) { cbData_.intensity = intensity; }
		
		/// <summary>
		/// シフトの中心を設定
		/// </summary>
		/// <param name="center">中心</param>
		void SetCenter(const Vector2& center) { cbData_.center = center; }

	private:

		struct RGBShiftCB
		{
			Vector2 center;
			float intensity;
			float padding;
		};

		RGBShiftCB cbData_;
		RGBShiftCB* mappedCB_ = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;

	};

}