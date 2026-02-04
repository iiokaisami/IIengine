#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <string>

namespace IIEngine
{

	class DirectXCommon;
	class SrvManager;

	/// <summary>
	/// ポストエフェクトパスのインターフェース
	/// 継承して各ポストエフェクトパスを実装する
	/// </summary>
	class IPostEffectPass
	{
	public:

		// デストラクタ
		virtual ~IPostEffectPass() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="dxCommon">DirectX共通機能管理クラスへのポインタ</param>
		/// <param name="srvManager">シェーダリソースビュー管理クラスへのポインタ</param>
		/// <param name="vsPath">頂点シェーダファイルのパス</param>
		/// <param name="psPath">ピクセルシェーダファイルのパス</param>
		virtual void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath) = 0;

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="cmdList">描画コマンドリスト</param>
		/// <param name="inputSrvHandle">入力テクスチャのSRVハンドル</param>
		/// <param name="inputResource">入力リソース</param>
		/// <param name="currentState">入力リソースの現在の状態</param>
		virtual void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrvHandle, ID3D12Resource* inputResource, D3D12_RESOURCE_STATES& currentState) = 0;

	public: // セッター

		/// <summary>
		/// アクティブ設定
		/// </summary>
		/// <param name="active">アクティブフラグ</param>
		virtual void SetActive(bool active) { isActive_ = active; }


	public: // ゲッター

		/// <summary>
		/// アクティブ取得
		/// </summary>
		/// <returns>アクティブフラグ</returns>
		virtual bool IsActive() const { return isActive_; }

		/// <summary>
		/// パス名取得
		/// </summary>
		/// <returns>パス名</returns>
		virtual std::string GetName() const = 0;

	protected:

		bool isActive_ = true;

	};

}