#pragma once
#include "IPostEffects.h"

class Random : public IPostEffects {
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
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	struct RandomFunction {
		float randomeTimer;
	};

	RandomFunction* randomFunction;

	Microsoft::WRL::ComPtr<ID3D12Resource> RandomResoure;
};