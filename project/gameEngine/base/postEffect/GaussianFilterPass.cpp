#include "GaussianFilterPass.h"

#include "../DirectXCommon.h"
#include "../SrvManager.h"

namespace IIEngine
{

    void GaussianFilterPass::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath)
    {
        BasePostEffectPass::Initialize(dxCommon, srvManager, vsPath, psPath);

        constantBuffer_ = dxCommon->CreateUploadBuffer(sizeof(cbData_));

        HRESULT hr = constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedCB_));
        assert(SUCCEEDED(hr) && "Failed to map constant buffer");
        hr;

        cbData_.intensity = 3.0f; // 初期値
    }

    void GaussianFilterPass::Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrvHandle, ID3D12Resource* inputResource, D3D12_RESOURCE_STATES& currentState)
    {
        currentState;
        inputResource;

        if (!isActive_)
        {
            return;
        }

        // CBV(定数バッファ)のアップロード等
        UpdateConstantBuffer();

        cmdList->SetPipelineState(pipelineState_.Get());
        cmdList->SetGraphicsRootSignature(rootSignature_.Get());
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        ID3D12DescriptorHeap* heaps[] = {
        srvManager_->GetHeap().Get(),
        dxCommon_->GetSamplerHeap().Get()
        };
        cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

        cmdList->SetGraphicsRootDescriptorTable(0, inputSrvHandle);
        cmdList->SetGraphicsRootDescriptorTable(1, dxCommon_->GetSamplerHandle());
        cmdList->SetGraphicsRootConstantBufferView(2, constantBuffer_->GetGPUVirtualAddress());

        cmdList->DrawInstanced(3, 1, 0, 0);
    }

    void GaussianFilterPass::UpdateConstantBuffer()
    {
        if (mappedCB_)
        {
            *mappedCB_ = cbData_;
        }
    }
}