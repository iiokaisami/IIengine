#pragma once

#include "IPostEffectPass.h"

#include <wrl.h>
#include <d3d12.h>
#include <string>

namespace IIEngine
{

    class DirectXCommon;
    class SrvManager;

    /// <summary>
    /// ポストエフェクトパスの基底クラス
    /// 継承して使用する
    /// </summary>
    class BasePostEffectPass : public IPostEffectPass {
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
        void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrvHandle, ID3D12Resource* inputResource, D3D12_RESOURCE_STATES& currentState) override = 0;

        /// <summary>
        /// リソースの状態を遷移させる
        /// </summary>
        /// <param name="cmdList">コマンドリスト</param>
        /// <param name="resource">状態を遷移させたいリソース</param>
        /// <param name="currentState">現在のリソース状態</param>
        /// <param name="requiredState">必要なリソース状態</param>
        void EnsureResourceState(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, ID3D12Resource* resource, D3D12_RESOURCE_STATES& currentState, D3D12_RESOURCE_STATES requiredState);

    protected:

        DirectXCommon* dxCommon_ = nullptr;
        SrvManager* srvManager_ = nullptr;

        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
    };
}