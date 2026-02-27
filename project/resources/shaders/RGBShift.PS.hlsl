#include "RGBShift.hlsli"

cbuffer RGBShiftCB : register(b0)
{
    float2 center; // 画面中心
    float intensity; // ずらし強度
    float padding;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float2 uv = input.texcoord;

    // 中心からの方向ベクトル
    float2 dir = uv - center;

    // 距離に応じて強度変化
    float dist = length(dir);
    float2 offset;
    if (dist > 1e-6f)
    {
        offset = (dir / dist) * intensity * dist;
    }
    else
    {
        offset = float2(0.0f, 0.0f);
    }
    
    // 各色チャンネルを別々にサンプリング
    float r = gTexture.Sample(gSampler, uv + offset).r;
    float g = gTexture.Sample(gSampler, uv).g;
    float b = gTexture.Sample(gSampler, uv - offset).b;

    PixelShaderOutput o;
    o.color = float4(r, g, b, 1.0f);
    return o;
}
