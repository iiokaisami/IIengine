#include "Scanline.hlsli"

cbuffer ScanlineCB : register(b0)
{
    float intensity;
    float density;
    float speed;
    float time;
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

    float scan = sin(input.texcoord.y * density + time * speed);
    scan = scan * 0.5f + 0.5f;

    float stripe = step(0.8f, scan);
    
    float mask = lerp(1.0f, 0.3f, stripe * intensity);
    color.rgb *= mask;

    PixelShaderOutput o;
    o.color = color;
    return o;
}
