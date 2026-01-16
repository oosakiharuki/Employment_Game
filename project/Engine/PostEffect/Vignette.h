#pragma once
#include "BasePostEffect.h"
/// <summary>
/// ポストエフェクト_Vignette(IPostEffectの派生クラス)
/// </summary>
class Vignette : public BasePostEffect {
public:
	void Finalize() override;
	void Command() override;
private:
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	void RootSignature() override;

	/// <summary>
	/// PixelShaderを作成(ポストエフェクトはこれ以外は共通)
	/// </summary>
	void CreatePixelShader() override;

	/// <summary>
	/// ポストエフェクトの初期化処理
	/// </summary>
	void EffectInit() override;

	/// <summary>
	/// ポストエフェクトの更新処理
	/// </summary>
	void EffectUpdate() override;

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