#pragma once

#include <wrl.h>

#include "BasePostEffectPass.h"

#include <MyMath.h>

namespace IIEngine
{
	/// <summary>
	/// クロマティックパルスエフェクトパス
	/// </summary>
	class ChromaticPulsePass : public BasePostEffectPass
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
		std::string GetName() const override { return "ChromaticPulse"; }

		/// <summary>
		/// 定数バッファ更新
		/// </summary>
		void UpdateConstantBuffer();

		/// <summary>
		/// パルスをトリガー
		/// </summary>
		void TriggerPulse();

	public: // セッター

		/// <summary>
		///	中心を設定
		/// </summary>
		/// <param name="c">中心</param>
		void SetCenter(const Vector2& c) { cbData_.center = c; }
		
		/// <summary>
		/// 半径を設定
		/// </summary>
		/// <param name="r">半径</param>
		void SetRadius(float r) { cbData_.radius = r; }
		
		/// <summary>
		/// 幅を設定
		/// </summary>
		/// <param name="w">幅</param>
		void SetWidth(float w) { cbData_.width = w; }
		
		/// <summary>
		/// 強さを設定
		/// </summary>
		/// <param name="i">強さ</param>
		void SetIntensity(float i) { cbData_.intensity = i; }

		/// <summary>
		/// 周波数を設定
		/// </summary>
		/// <param name="f">周波数</param>
		void SetFrequency(float f) { cbData_.frequency = f; }
		
		/// <summary>
		/// 速度を設定
		/// </summary>
		/// <param name="s">速度</param>
		void SetSpeed(float s) { cbData_.speed = s; }

	private:

		struct ChromaticPulseCB
		{
			Vector2 center;
			float radius;
			float width;
			float intensity;

			float time;
			float frequency;
			float speed;
		};


		ChromaticPulseCB cbData_;
		ChromaticPulseCB* mappedCB_ = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;

		float pulseDuration_ = 0.5f;
		bool  isPulsing_ = false;

	};

}