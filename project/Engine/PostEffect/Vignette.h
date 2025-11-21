#pragma once
#include "IPostEffects.h"
/// <summary>
/// ポストエフェクト_Vignette(IPostEffectの派生クラス)
/// </summary>
class Vignette : public IPostEffects {
public:
	void Finalize() override;

	void Initialize(DirectXCommon* dxCommon) override;
	DirectXCommon* GetDirectXCommon()const { return dxCommon_; }

	void Command() override;
private:
	//PSO
	void RootSignature() override;
	void GraphicsPipeline() override;

	void EffectUpdate() override;

	DirectXCommon* dxCommon_;

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[2] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};

	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateDepth_ = nullptr;

	uint32_t srvIndex_;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;

	struct VignetteFunction {
		float luminance;
		float darkness;
	};

	VignetteFunction* vignetteFunction_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vignetteResource_;
};