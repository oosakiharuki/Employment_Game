#pragma once
#include "IPostEffects.h"
/// <summary>
/// ポストエフェクト_通常、何もなし(IPostEffectの派生クラス)
/// </summary>
class Normal_Image : public IPostEffects {
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
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[1] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
};