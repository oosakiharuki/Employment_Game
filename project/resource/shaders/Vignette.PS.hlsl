#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Material
{
    float luminance;
    float darkness;
};

ConstantBuffer<Material> gMaterial : register(b0);

PixelShaderOutput main(VartexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //中心に近いほど明るくなる(ここはほぼ動かさなそう)
    float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    
    //correctの値が小さすぎなので大きくする
    float vignette = correct.x * correct.y * gMaterial.luminance;
    
    //pow = vignetteの○乗
    vignette = saturate(pow(vignette, gMaterial.darkness));
    
    output.color.rgb *= vignette;
    
    return output;
}