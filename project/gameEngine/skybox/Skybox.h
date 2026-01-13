#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <array>
#include <DirectXMath.h>
#include <MyMath.h>


class DirectXCommon;
class SrvManager;

namespace IIEngine
{

    struct SkyboxVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 texcoord;
    };

    struct Material
    {
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT4X4 uvTransform;
    };

    /// <summary>
    /// スカイボックス
    /// </summary>
    class Skybox
    {
    public:

        /// <summary>
        /// 初期化
        /// </summary>
        /// <param name="dxCommon">DirectX共通管理</param>
        void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
        // 終了
        void Finalize();
        // 更新
        void Update();
        // 描画
        void Draw();

    public: // セッター

        /// <summary>
        /// キューブマップのSRVインデックスを設定
        /// </summary>
        /// <param name="index">SRVインデックス</param>
        void SetCubeMapSrvIndex(uint32_t index) { cubeMapSrvIndex_ = index; }

    private:

        // 頂点・インデックスバッファの生成
        void CreateBuffers();
        // 定数バッファの生成
        void CreateConstantBuffer();
        // ルートシグネチャの生成
        void CreateRootSignature();
        // パイプラインステートの生成
        void CreatePipelineState();

    private:

        // 24頂点と36インデックスを持つ立方体の頂点データ
        std::array<SkyboxVertex, 24> vertexData_{};
        std::array<uint16_t, 36> indexData_{};

        // 頂点・インデックスバッファ
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
        Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;
        D3D12_VERTEX_BUFFER_VIEW vbView_{};
        D3D12_INDEX_BUFFER_VIEW ibView_{};

        // 定数バッファ（ビュー・射影行列など）
        Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
        struct ConstBufferData {
            Matrix4x4 viewProj;
        };
        ConstBufferData* mappedConstBuffer_ = nullptr;

        Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer_;
        Material* mappedMaterial_ = nullptr;

        // ルートシグネチャ
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
        // パイプラインステート
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

        // SRVインデックス（SrvManager等で割り当てたCubeMapのSRV番号）
        uint32_t cubeMapSrvIndex_ = 0;

        // DirectX共通管理
        DirectXCommon* dxCommon_ = nullptr;
        // SRVマネージャー
        SrvManager* srvManager_ = nullptr;
    };

}