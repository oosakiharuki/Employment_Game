#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);

struct Threshold
{
    float32_t degress;
    float32_t egdeSize;
    float32_t4 color;
};

ConstantBuffer<Threshold> gThreshold : register(b0);

SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VartexShaderOutput input)
{
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    if (mask <= gThreshold.degress)
    {
        discard;
    }
    
    PixelShaderOutput output;

    //Edge演出
    float32_t edge = 1.0f - smoothstep(gThreshold.degress, gThreshold.degress + gThreshold.egdeSize, mask); //0.03fは色を付けられる場所
    
    output.color = gTexture.Sample(gSampler, input.texcoord);  
    output.color.rgb += edge * float32_t3(1.0f, 0.4f, 0.3f);
    return output;
}