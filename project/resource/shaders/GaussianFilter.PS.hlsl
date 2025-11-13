#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

float gauss(float x, float y, float sigma)
{
    static const float32_t PI = 3.14159265f;
                
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
            
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VartexShaderOutput input)
{
    static const int32_t kBox = 9;
    float32_t2 kIndex3x3[kBox][kBox];
    
    float k = (kBox - 1) / 2;
    //-1,0,1の順番
    for (int32_t i = -k; i <= k; ++i)
    {
        for (int32_t j = -k; j <= k; ++j)
        {
            
            kIndex3x3[k + i][k + j].x = i;
            kIndex3x3[k + i][k + j].y = j;
        }
    }
    
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height)); //rcp = 逆数
    
    PixelShaderOutput output;
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;

    float32_t weight = 0.0f;
    float32_t Kernel3x3[kBox][kBox];
    
    for (int32_t x = 0; x < kBox; ++x)
    {
        for (int32_t y = 0; y < kBox; ++y)
        {
            
            Kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 2.0f);
            weight += Kernel3x3[x][y];

            float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * Kernel3x3[x][y];
        }

    }
    
    output.color.rgb *= rcp(weight);
    
    return output;
}