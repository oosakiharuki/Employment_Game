#pragma once
#include "DirectXCommon.h"
class Camera;

class SkinningCommon {
public:
	static SkinningCommon* GetInstance();

	void Initialize(DirectXCommon* dxCommon);
	void Finalize();
	DirectXCommon* GetDirectXCommon()const { return dxCommon_; }

	void Command();

	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
	Camera* GetDefaultCamera() const { return defaultCamera; }

private:
	//PSO
	void RootSignature();
	void GraphicsPipeline();

	DirectXCommon* dxCommon_ = nullptr;

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRangeIBL[1] = {};//iamge_based_lighting
	D3D12_ROOT_PARAMETER rootParameters[9] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};



	//バイナリを元に生成
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature = nullptr;

	//PSO
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;


	Camera* defaultCamera = nullptr;

	static SkinningCommon* instance;

	SkinningCommon() = default;
	~SkinningCommon() = default;
	SkinningCommon(SkinningCommon&) = default;
	SkinningCommon& operator=(SkinningCommon&) = default;


	static uint32_t kSRVIndexTop;
};