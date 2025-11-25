#pragma once
#include "IPostEffects.h"
#include "Vector3.h"
/// <summary>
/// ポストエフェクト_Dissolve(IPostEffectの派生クラス)
/// </summary>
class Dissolve : public IPostEffects {
public:
	void Finalize() override;
	void Initialize(DirectXCommon* dxCommon) override;
	void Command() override;

	void SetBackGround(D3D12_GPU_DESCRIPTOR_HANDLE gpu,const std::string textureFile);
	void Degress(float value);
	void EdgeSize(float value);

	struct Threshold {
		float degress;
		float edgeSize;
	};

private:
	//PSO
	void RootSignature() override;
	void GraphicsPipeline() override;

	void EffectUpdate() override;

	//RootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRangeDissolve_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[3] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};

	std::string textureFileName_;

	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> backGroundResource_;

	D3D12_CPU_DESCRIPTOR_HANDLE TextureCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE TextureGPU_;
	bool isFade_ = false;


	Threshold* threshold_;
	bool isChangeMask_ = false;
};