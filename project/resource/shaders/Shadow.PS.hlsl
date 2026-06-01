#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t isEnable;
    float32_t4x4 uvTransform; 
};

struct ShadowPoint
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float32_t3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
};

struct Camera
{
    float32_t3 worldPosition;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<ShadowPoint> gShadowPoint : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);

struct PixelShaderOutput
{
    float32_t4 color : SV_Target0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    
    PixelShaderOutput output;
    
    
    
    if (gMaterial.isEnable)
    {
        
        flaot32_t3 toEye = normalize(gCamera.worldPosition - input.worldPostion);
        
        
        //入射
        float NdotL = dot(normalize(input.normal), -gShadowPoint.position);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        
        float onSerfec = normalize(input.worldPostion, gShadowPoint.direction);
        
        float32_t cosAngle = dot(NdotL, gShadowPoint.direction);
        //減衰(段々弱くなる)
        float32_t falloffFactor = saturate((cosAngle - gShadowPoint.cosAngle) / (1.0f - gShadowPoint.cosAngle));
        
 

        float32_t3 diffuse = gMaterial.color.rgb * gShadowPoint.intensity * cos;
        float32_t3 specular;

        output.color.rgb = gMaterial.color.rgb - (diffuse * specular);

    }
    else
    {
        output.color = gMaterial.color;
    }
    
    
}