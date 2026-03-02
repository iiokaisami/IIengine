#include "Inversion.hlsli"

cbuffer InversionCB : register(b0)
{
    float intensity;
    float3 padding;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 color = gTexture.Sample(gSampler, input.texcoord);

    PixelShaderOutput o;
    o.color = float4(1.0 - color.rgb, 1.0); // 常に反転
    return o;
}
