#pragma once
#include "IPostEffects.h"
#include "MyMath.h"

class DepthBasedOutline : public IPostEffects {
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
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRangeOutline[1] = {};
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};

	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateDepth = nullptr;

	uint32_t srvIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU2;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU2;

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;

	struct DepthOutlineFunction {
		float projectionInverse;
	};
	
	DepthOutlineFunction* depthOutlineFunction = nullptr;

};