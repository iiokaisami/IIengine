#include "ChromaticPulsePass.h"

#include <DirectXCommon.h>
#include <SrvManager.h>
#include <TimeManager.h>

namespace IIEngine
{

    void ChromaticPulsePass::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, const std::wstring& vsPath, const std::wstring& psPath)
    {
        BasePostEffectPass::Initialize(dxCommon, srvManager, vsPath, psPath);

        cbData_.center = { 0.5f, 0.5f };
        cbData_.radius = 0.30f;
        cbData_.width = 0.05f;
        cbData_.intensity = 1.0f;
        cbData_.time = 0.0f;
        cbData_.frequency = 40.0f;
        cbData_.speed = 8.0f;

        constantBuffer_ = dxCommon->CreateUploadBuffer(sizeof(cbData_));
        HRESULT hr = constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedCB_));
        assert(SUCCEEDED(hr) && "Failed to map constant buffer");
        hr;
    }

    void ChromaticPulsePass::Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrvHandle, ID3D12Resource* inputResource, D3D12_RESOURCE_STATES& currentState)
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

    void ChromaticPulsePass::UpdateConstantBuffer()
    {
        const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();


        if (isPulsing_)
        {
            cbData_.time += dt;

            float t = cbData_.time / pulseDuration_;
            t = std::clamp(t, 0.0f, 1.0f);
            cbData_.intensity = (1.0f - t) * 1.5f;

            // 半径を0→1へ拡大
            cbData_.radius = t;

            if (cbData_.time >= pulseDuration_)
            {
                isPulsing_ = false;
            }
        }

        float t = cbData_.time / pulseDuration_;
        t = std::clamp(t, 0.0f, 1.0f);
        cbData_.intensity = (1.0f - t) * 1.5f;

        if (mappedCB_)
        {
            *mappedCB_ = cbData_;
        }

    }

    void ChromaticPulsePass::TriggerPulse()
    {
        cbData_.time = 0.0f;
        isPulsing_ = true;
    }

}