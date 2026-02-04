#pragma once

#include "DirectXCommon.h"

namespace IIEngine
{

	/// <summary>
	/// SRVマネージャー
	/// SRVの確保、生成、管理を行う
	/// </summary>
	class SrvManager
	{
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="dxCommon">DirectX共通クラス</param>
		void Initialize(DirectXCommon* dxCommon);

		// 確保関数
		/// <summary>
		/// SRV確保
		/// </summary>
		/// <returns>確保したSRVインデックス</returns>
		uint32_t Allocate();
		/// <summary>
		/// SRV確保確認
		/// </summary>
		/// <returns>確保済みか</returns>
		bool IsAllocate() const;

		// 計算用関数
		/// <summary>
		/// CPUデスクリプタハンドル取得
		/// </summary>
		/// <param name="index">インデックス</param>
		/// <returns>CPUデスクリプタハンドル</returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
		/// <summary>
		/// GPUデスクリプタハンドル取得
		/// </summary>
		/// <param name="index">インデックス</param>
		/// <returns>GPUデスクリプタハンドル</returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

		/// <summary>
		/// SRV生成(テクスチャ用)
		/// </summary>
		/// <param name="srvIndex">SRVインデックス</param>
		/// <param name="pResource">リソースポインタ</param>
		/// <param name="Format">フォーマット</param>
		/// <param name="MipLevels">ミップレベル数</param>
		/// <param name="metadata">テクスチャメタデータ</param>
		void CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels, const DirectX::TexMetadata& metadata);

		/// <summary>
		/// SRV生成(Structured Buffer用)
		/// </summary>
		/// <param name="srvIndex">SRVインデックス</param>
		/// <param name="pResource">リソースポインタ</param>
		/// <param name="numElements">要素数</param>
		/// <param name="structureByteStride">構造体バイトストライド</param>
		void CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

		// 描画前処理
		void PreDraw();

		/// <summary>
		/// テクスチャ読み込み
		/// </summary>
		/// <param name="textureFilePath">テクスチャファイルパス</param>
		/// <returns>SRVインデックス</returns>
		uint32_t LoadTexture(const std::string& textureFilePath);

	public: // ゲッター

		/// <summary>
		/// SRVのCPUディスクリプタハンドルを取得
		/// </summary>
		/// <param name="srvIndex">SRVインデックス</param>
		/// <returns>SRVのCPUディスクリプタハンドル</returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t srvIndex) const
		{
			if (srvIndex >= kMaxSRVCount_) {
				throw std::out_of_range("Invalid SRV index");
			}
			D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
			handle.ptr += descriptorSize_ * srvIndex;
			return handle;
		}

		/// <summary>
		/// SRV用デスクリプタヒープを取得
		/// </summary>
		/// <returns>SRV用デスクリプタヒープ</returns>
		Microsoft::WRL::ComPtr <ID3D12DescriptorHeap> GetHeap() const
		{
			return descriptorHeap_;
		}

	public: // セッター

		/// <summary>
		/// グラフィックスルートデスクリプタテーブルを設定
		/// </summary>
		/// <param name="RootParameterIndex">ルートパラメータインデックス</param>
		/// <param name="srvIndex"> SRVインデックス</param>
		void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	private:

		DirectXCommon* dxCommon_ = nullptr;

		// 最大SRV数(最大テクスチャ数)
		static const uint32_t kMaxSRVCount_;
		//SRV用デスクリプタサイズ
		uint32_t descriptorSize_{};
		// SRV用デスクリプタヒープ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		// 次に使用するSRVインデックス
		uint32_t useIndex_ = 0;

		// テクスチャキャッシュ
		std::unordered_map<std::string, uint32_t> textureIndices_;

	};

}