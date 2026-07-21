#include"Object3d.hlsli"


//float4 main() : SV_TARGET
//{
//    return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
    float32_t environmentCoefficient;
};

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
    int32_t isEnable;
};

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float radius;
    float decay;
    int32_t isEnable;
};

struct SpotLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float32_t3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
    int32_t isEnable;
};

struct ShadowData
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

static const int32_t kMaxShadow = 60;

struct ShadowFactory
{
    ShadowData shadow[kMaxShadow];
};


struct Camera
{
    float32_t3 worldPosition;
};


ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);
ConstantBuffer<ShadowFactory> gShadowFactory : register(b5);

Texture2D<float32_t4> gTexture : register(t0);
TextureCube<float32_t4> gEnvironmentTexture : register(t1);

SamplerState gSampler : register(s0);



struct PixelShaderOutput
{
    float32_t4 color : SV_Target0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformdUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformdUV.xy);

    PixelShaderOutput output;
        
    // --- カメラ ---
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPostion);
  
    // --- オブジェクト初期 ---
    output.color.rgb = (gMaterial.color.rgb * textureColor.rgb);
    
    if (gMaterial.enableLighting != 0)
    {
        output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
        
        // --- 平行光源 ---
             
        if (gDirectionalLight.intensity != 0)
        {
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            
            float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
            float NdotH = dot(normalize(input.normal), halfVector);
            float specularPow = pow(saturate(NdotH), gMaterial.shininess); // saturate (u * r) s乗
            
            float32_t3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            float32_t3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
            
            output.color.rgb += diffuseDirectionalLight + specularDirectionalLight;

        }
        
        // --- 平行光源end ---
        
        
        // --- ポイントライト ---

        if (gPointLight.intensity != 0)
        {
            float NdotL = dot(normalize(input.normal), -gPointLight.position);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            
            float32_t3 halfVectorP = normalize(gPointLight.position + toEye);
            float NdotHP = dot(normalize(input.normal), halfVectorP);
            float specularPowP = pow(saturate(NdotHP), gMaterial.shininess); // saturate (u * r) s乗
            
            float32_t distance = length(gPointLight.position - input.worldPostion);
            float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);

            float32_t3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
            float32_t3 specularPointLight = gPointLight.color.rgb * gPointLight.intensity * factor * specularPowP * float32_t3(1.0f, 1.0f, 1.0f);
            
            output.color.rgb += diffusePointLight + specularPointLight;

        }
        // --- ポイントライトend ---
        
        // --- スポットライト ---

        if (gSpotLight.intensity != 0)
        {
            float NdotL = dot(normalize(input.normal), -gSpotLight.direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            
            float32_t3 halfVectorS = normalize(gSpotLight.position + toEye);
            float NdotHS = dot(normalize(input.normal), halfVectorS);
            float specularPowS = pow(saturate(NdotHS), gMaterial.shininess); // saturate (u * r) s乗
            
            float32_t3 spotLightDirectionOnSurface = normalize(input.worldPostion - gSpotLight.position);
        
            float32_t cosAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
            float32_t falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (1.0f - gSpotLight.cosAngle)); //フォールオフ              
            float32_t falloffFactorS = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));  
        
            float32_t distanceSpot = length(gSpotLight.position - input.worldPostion); //ライトとオブジェクトの距離
            float32_t attenuationFactor = pow(saturate(-distanceSpot / gSpotLight.distance + 1.0f), gSpotLight.decay); //距離による減衰
        
            float32_t3 diffuseSpotLight = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cos * gSpotLight.intensity * attenuationFactor;
            float32_t3 specularSpotLight = gSpotLight.color.rgb * gSpotLight.intensity * attenuationFactor * falloffFactor * specularPowS * float32_t3(1.0f, 1.0f, 1.0f);
        
            output.color.rgb += diffuseSpotLight + specularSpotLight;
        }
        
        // --- スポットライトend ---


        //float32_t3 cameraToPosition = normalize(input.worldPostion - gCamera.worldPosition);//向き
        //float32_t3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));//反射ベクトル
        //float32_t4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);//Cubemapのテクスチャ
        
        //output.color.rgb += environmentColor.rgb * gMaterial.environmentCoefficient;//映り込み度
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
       
    // --- 影 ---
        
    for (int i = 0; i < kMaxShadow; ++i)
    {
        if (gShadowFactory.shadow[i].intensity != 0)
        {
            float NdotL = dot(normalize(input.normal), -gShadowFactory.shadow[i].direction);
            float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
            
            float32_t3 halfVectorS = normalize(gShadowFactory.shadow[i].position + toEye);
            float NdotHS = dot(normalize(input.normal), halfVectorS);
            float specularPowS = pow(saturate(NdotHS), gMaterial.shininess); // saturate (u * r) s乗
            
            float32_t3 spotLightDirectionOnSurface = normalize(input.worldPostion - gShadowFactory.shadow[i].position);
        
            float32_t cosAngle = dot(spotLightDirectionOnSurface, gShadowFactory.shadow[i].direction);
            float32_t falloffFactor = saturate((cosAngle - gShadowFactory.shadow[i].cosAngle) / (1.0f - gShadowFactory.shadow[i].cosAngle)); //フォールオフ              
            float32_t falloffFactorS = saturate((cosAngle - gShadowFactory.shadow[i].cosAngle) / (gShadowFactory.shadow[i].cosFalloffStart - gShadowFactory.shadow[i].cosAngle));
        
            float32_t distanceSpot = length(gShadowFactory.shadow[i].position - input.worldPostion); //ライトとオブジェクトの距離
            float32_t attenuationFactor = pow(saturate(-distanceSpot / gShadowFactory.shadow[i].distance + 1.0f), gShadowFactory.shadow[i].decay); //距離による減衰
        
            float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * cos * gShadowFactory.shadow[i].intensity * attenuationFactor * falloffFactorS;
            
                //壁であるならうつさない
            if (input.normal.y > 0.25f)
            {
                output.color.rgb -= diffuse; //減算して暗く
            }
        }
    }
    
    output.color.a = gMaterial.color.a * textureColor.a;
    
    // --- 影end ---
    
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
    if (textureColor.a == 0.0)
    {
        discard;
    }
    
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}