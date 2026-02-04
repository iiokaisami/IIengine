#pragma once

#include <wrl.h>
#include <d3d12.h>  
#include <cstdint>
#include "MyMath.h"

namespace IIEngine
{

    class DirectXCommon;
    class SrvManager;

    /// <summary>
    /// レンダーターゲットとして使うテクスチャ
    /// </summary>
    class RenderTexture
    {
    public:

        /// <summary>
        /// 初期化
        /// </summary>
        /// <param name="dxCommon"> dx共通クラス</param>
        /// <param name="srvManager"> SRVマネージャー</param>
        /// <param name="width">ウィンドウ幅</param>
        /// <param name="height">ウィンドウ高さ</param>
        /// <param name="format">フォーマット</param>
        /// <param name="clearColor">クリアカラー</param>
        void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);
        /// <summary>
        /// レンダーターゲットとして使用開始
        /// </summary>
        void BeginRender();
        /// <summary>
        /// レンダーターゲットとして使用終了
        /// </summary>
        void EndRender();


    public: // ゲッター

        // リソースの取得
        ID3D12Resource* GetResource() const { return texture_.Get(); }

        // RTV ハンドルの取得
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;

        // SRV インデックスの取得
        uint32_t GetSRVIndex() const { return srvIndex_; }

        // SRV ハンドルの取得
        D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() const;

        // 現在のリソース状態
        D3D12_RESOURCE_STATES& GetCurrentState() { return currentState_; }

    private:

        DirectXCommon* dxCommon_ = nullptr;
        SrvManager* srvManager_ = nullptr;

        Microsoft::WRL::ComPtr<ID3D12Resource> texture_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{};
        uint32_t srvIndex_ = 0;
        DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        Vector4 clearColor_ = { 0, 0, 0, 1 };
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

    };
}