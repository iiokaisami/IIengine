#include "ChromaticPulse.hlsli"

cbuffer ChromaticPulseCB : register(b0)
{
    float2 center;
    float radius;
    float width;
    float intensity;
    
    float time;
    float frequency;
    float speed;
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
    float4 base = gTexture.Sample(gSampler, uv);

    float2 dir = uv - center;
    float dist = length(dir);

    float pulse = sin(dist * frequency - time * speed);
    float ring = smoothstep(radius - width, radius, dist) - smoothstep(radius, radius + width, dist);

    float2 offset = float2(0.0f, 0.0f);
    if (dist > 1e-6f)
    {
        offset = (dir / dist) * (pulse * ring * intensity * 0.02f);
    }

    float r = gTexture.Sample(gSampler, uv + offset).r;
    float g = base.g;
    float b = gTexture.Sample(gSampler, uv - offset).b;

    PixelShaderOutput o;
    o.color = float4(r, g, b, base.a);
    return o;
}
