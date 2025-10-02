#pragma once
#include "IPostEffects.h"

class  RadialBlur : public IPostEffects {
public:	
	void Finalize() override;
	void Initialize(DirectXCommon* dxCommon) override;
	void Command() override;
private:
	//PSO
	void RootSignature() override;
	void GraphicsPipeline() override;

	void EffectUpdate() override;

	//RootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

};