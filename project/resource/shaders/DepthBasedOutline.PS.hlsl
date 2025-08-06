#include "Fullscreen.hlsli"

struct Material
{
    float32_t projectionInverse;
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

Texture2D<float32_t> gDepthTexture : register(t1);
SamplerState gSamplerPoint : register(s1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

float32_t Luminance(float32_t3 v)
{
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}


PixelShaderOutput main(VartexShaderOutput input)
{
    static const int32_t box = 3;
    float32_t2 kIndex3x3[box][box];
    
    float k = (box - 1) / 2;
    //-1,0,1の順番
    for (int32_t i = -k; i <= k; ++i)
    {
        for (int32_t j = -k; j <= k; ++j)
        {
            
            kIndex3x3[k + i][k + j].x = i;
            kIndex3x3[k + i][k + j].y = j;
        }
    }
     
    // (自身の面{0,0}) 1 / 9 (縦横斜めの面8+自身の面1)
    //h[][]
    float32_t kPrewittHorisontalKernel[3][3] =
    {
        { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
        { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
        { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    };

    float32_t kPrewittVerticalKernel[3][3] =
    {
        { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
        { 0.0f, 0.0f, 0.0f },
        { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
    };
    
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height)); //rcp = 逆数
    

    float32_t2 difference = float32_t2(0.0f, 0.0f);
    
    for (int32_t x = 0; x < 3; ++x)
    {
        for (int32_t y = 0; y < 3; ++y)
        {
            float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            float32_t ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
            
            float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f),gMaterial.projectionInverse);
            ///viewSpace.x = 0 * proI + 0 * proI + ndcDepth * proI + 1.0f * proI ...
            float32_t viewZ = viewSpace.z * viewSpace.w; //viewSpaceの-1乗
            
            difference.x += viewZ * kPrewittHorisontalKernel[x][y];
            difference.y += viewZ * kPrewittVerticalKernel[x][y];
        }
    }
    float32_t weight = length(difference);
    
    weight = saturate(weight); //0 ～ 1
    
    
    PixelShaderOutput output;
    
    output.color.rgb = (1 - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;

    
    return output;
}