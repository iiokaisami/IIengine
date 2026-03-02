#include "Noise.hlsli"

cbuffer NoiseCB : register(b0)
{
    float intensity;
    float time;
    float2 padding;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

float rand(float2 co)
{
    return frac(sin(dot(co, float2(12.9898, 78.233))) * 43758.5453);
}


struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 color = gTexture.Sample(gSampler, input.texcoord);

    float noise = rand(input.texcoord * 500.0f + time * 10.0f);
    noise = (noise - 0.5f) * 2.0f; // -1～1
    noise *= intensity * 0.1f;

    color.rgb += noise;

    PixelShaderOutput o;
    o.color = color;
    return o;
}
