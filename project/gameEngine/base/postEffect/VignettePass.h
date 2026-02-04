#pragma once

#include "BasePostEffectPass.h"

#include <d3d12.h>
#include <wrl/client.h>
#include <string>
#include <MyMath.h>

namespace IIEngine
{

	/// <summary>
	/// ビネット処理
	/// カメラの周辺を暗くする
	/// </summary>
	class VignettePass :public BasePostEffectPass
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
		/// コンスタントバッファ更新
		/// </summary>
		void UpdateConstantBuffer();

	public: // セッター

		/// <summary>
		/// 強さ設定
		/// </summary>
		/// <param name="strength">強さ</param>
		void SetStrength(float strength) { cbData_.strength = strength; }

	public: // ゲッター

		/// <summary>
		/// エフェクト名取得
		/// </summary>
		std::string GetName() const override { return "Vignette"; }


	private:

		struct VignetteCB
		{
			float strength = 1.0f;
			float padding[3];
		};

		VignetteCB cbData_ = {};
		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
		VignetteCB* mappedCB_ = nullptr;

	};

}