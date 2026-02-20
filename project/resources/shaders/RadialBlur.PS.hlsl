#include "RadialBlur.hlsli"

cbuffer RadialBlurCB : register(b0)
{
    float2 center; // ブラー中心
    float strength; // ブラー強度
    int sampleCount; // サンプル数
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float2 dir = input.texcoord - center;

    float4 sum = 0;
    float weightSum = 0;

    for (int i = 0; i < sampleCount; ++i)
    {
        float t = (float) i / (float) (sampleCount - 1);
        float2 uv = input.texcoord - dir * t * strength;

        float weight = 1.0f - t; // 中心側を強く
        sum += gTexture.Sample(gSampler, uv) * weight;
        weightSum += weight;
    }

    output.color = sum / weightSum;

    return output;
}