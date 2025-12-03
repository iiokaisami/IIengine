#include "Object3d.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
    int phongReflection;
    int halfphongReflection;
    int pointLight;
    int spotLight;
    int enableTexture;
    float environmentStrength;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};

struct SpotLight
{
    float4 color;
    float3 position;
    float intensity;
    float3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
};

struct Enviroment
{
    int enable;
    float strength;
};


ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);
Texture2D<float4> gTexture : register(t0);
TextureCube<float4> gEnvironmentTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // UV transform & texture sample
    float4 transformeduv = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformeduv.xy);

    // FIXED: output alpha should be based on material + texture only (not lighting)
    float outAlpha = gMaterial.color.a * textureColor.a;

    // normalize normal once
    float3 n = normalize(input.normal);

    // prepare reused vectors
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

    // Directional light helpers (normalized normal used)
    float3 reflectLight = reflect(gDirectionalLight.direction, n);
    float RtoE = dot(reflectLight, toEye);
    float specularPow = pow(saturate(RtoE), gMaterial.shininess);

    float NdotL = dot(n, -gDirectionalLight.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

    float3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    float3 specularDirectionalLight = gMaterial.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

    if (gMaterial.enableLighting != 0)
    {
        // CHANGED: lighting only affects RGB; alpha stays outAlpha
        float3 lit = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color = float4(lit, outAlpha);
    }
    else if (gMaterial.phongReflection != 0)
    {
        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight;
        output.color.a = outAlpha;
    }
    else if (gMaterial.halfphongReflection != 0)
    {
        // FIXED: compute half-vector / specular correctly and add to RGB; alpha = outAlpha
        float3 halfVector = normalize(-gDirectionalLight.direction + toEye);
        float NDotH = dot(n, halfVector);
        float specularPow2 = pow(saturate(NDotH), gMaterial.shininess);
        float3 specular = gMaterial.color.rgb * gDirectionalLight.intensity * specularPow2 * float3(1.0f, 1.0f, 1.0f);

        output.color.rgb = diffuseDirectionalLight + specular;
        output.color.a = outAlpha;
    }
    else if (gMaterial.pointLight != 0)
    {
        // Point light calculations
        float3 diffusePointLight = { 0.0f, 0.0f, 0.0f };
        float3 specularPointLight = { 0.0f, 0.0f, 0.0f };

        float3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
        float3 toEyePL = normalize(gCamera.worldPosition - input.worldPosition);
        float3 halfVectorPL = normalize(-pointLightDirection + toEyePL);
        float NdotHPL = dot(n, halfVectorPL);
        float specPowPL = pow(saturate(NdotHPL), gMaterial.shininess);

        float NdotLpl = dot(n, -pointLightDirection);
        float cospl = pow(NdotLpl * 0.5f + 0.5f, 2.0f);
        float distance = length(gPointLight.position - input.worldPosition);
        float factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);

        diffusePointLight += gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cospl * gPointLight.intensity * factor;
        float3 specularColor = { 1.0f, 1.0f, 1.0f };
        specularPointLight += gPointLight.color.rgb * gPointLight.intensity * specPowPL * specularColor * factor;

        output.color.rgb = diffusePointLight + specularPointLight;
        output.color.a = outAlpha;
    }
    else if (gMaterial.spotLight != 0)
    {
        // Spot light calculations
        float3 diffuseSpotLight = { 0.0f, 0.0f, 0.0f };
        float3 specularSpotLight = { 0.0f, 0.0f, 0.0f };

        float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
        float3 toEyeSL = normalize(gCamera.worldPosition - input.worldPosition);
        float3 halfVectorSL = normalize(-spotLightDirectionOnSurface + toEyeSL);
        float NdotHSL = dot(n, halfVectorSL);
        float specularPowSL = pow(saturate(NdotHSL), gMaterial.shininess);

        float NdotLsl = dot(n, -spotLightDirectionOnSurface);
        float cossl = pow(NdotLsl * 0.5f + 0.5f, 2.0f);
        float distance = length(gSpotLight.position - input.worldPosition);
        float attenuationFactor = pow(saturate(-distance / gSpotLight.distance + 1.0f), gSpotLight.decay);
        float cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
        float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));

        diffuseSpotLight += gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cossl * gSpotLight.intensity * attenuationFactor * falloffFactor;
        float3 specularColor = { 1.0f, 1.0f, 1.0f };
        specularSpotLight += gSpotLight.color.rgb * gSpotLight.intensity * specularPowSL * specularColor * attenuationFactor * falloffFactor;

        output.color.rgb = diffuseSpotLight + specularSpotLight;
        output.color.a = outAlpha;
    }
    else if (gMaterial.enableTexture != 0 && gMaterial.environmentStrength > 0.0f)
    {
        // Environment map (RGB only), alpha preserved
        float3 envAcc = float3(0.0f, 0.0f, 0.0f);
        float3 cameraToPosition = normalize(gCamera.worldPosition - input.worldPosition);
        float3 reflectedVector = reflect(-cameraToPosition, n);
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
        envAcc += environmentColor.rgb * gMaterial.environmentStrength;
        output.color = float4(envAcc, outAlpha);
    }
    else
    {
        // Unlit fallback - ensure alpha set consistently
        output.color = gMaterial.color * textureColor;
        output.color.a = outAlpha;
    }
    
    return output;
}
