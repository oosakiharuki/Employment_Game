#pragma once
#include "IPostEffects.h"
#include "MyMath.h"
/// <summary>
/// ポストエフェクト_DepthBasedOutline(IPostEffectの派生クラス)
/// </summary>
class DepthBasedOutline : public IPostEffects {
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
	D3D12_DESCRIPTOR_RANGE descriptorRangeOutline_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[3] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[2] = {};



	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	struct DepthOutlineFunction {
		float projectionInverse;
	};
	
	DepthOutlineFunction* depthOutlineFunction_ = nullptr;

};