#pragma once
#include "IPostEffects.h"
/// <summary>
/// ポストエフェクト_Vignette(IPostEffectの派生クラス)
/// </summary>
class Vignette : public IPostEffects {
public:
	void Finalize() override;
	void Command() override;
	DirectXCommon* GetDirectXCommon()const { return dxCommon_; }
private:
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	void RootSignature() override;

	/// <summary>
	/// PixelSharderを作成(ポストエフェクトはこれ以外は共通)
	/// </summary>
	void CreatePixelSharder() override;

	/// <summary>
	/// ポストエフェクトの初期化処理
	/// </summary>
	void EffectInit() override;

	/// <summary>
	/// ポストエフェクトの更新処理
	/// </summary>
	void EffectUpdate() override;

	//RootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[2] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};

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