#pragma once
#include "IPostEffects.h"
#include <Vector4.h>
/// <summary>
/// ポストエフェクト_Dissolve(IPostEffectの派生クラス)
/// </summary>
class Dissolve : public IPostEffects {
public:
	void Finalize() override;
	void Initialize(DirectXCommon* dxCommon) override;
	void Command() override;

	void SetBackGround(D3D12_GPU_DESCRIPTOR_HANDLE gpu);
	void Degress(float value);

	struct Threshold {
		float degress;
		float egdeSize;
		//Vector4 egdeColor;
	};

private:
	//PSO
	void RootSignature() override;
	void GraphicsPipeline() override;

	void EffectUpdate() override;

	//RootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRangeDissolve[1] = {};
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	std::string textureFileName;

	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> backGroundResource;

	D3D12_CPU_DESCRIPTOR_HANDLE TextureCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE TextureGPU;
	bool isFade = false;


	Threshold* threshold;
	bool isChangeMask = false;
};