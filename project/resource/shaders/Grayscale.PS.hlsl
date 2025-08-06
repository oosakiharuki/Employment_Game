#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct GrayFunction
{
    int32_t isSepia;
    float32_t3 color;
};

ConstantBuffer<GrayFunction> gMaterial : register(b0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VartexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //グレースケール
    float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
    output.color.rgb = float32_t3(value, value, value);
    
    //セピア調
    if (gMaterial.isSepia)
    {
        output.color.rgb = value * gMaterial.color;
    }
    
    return output;
}