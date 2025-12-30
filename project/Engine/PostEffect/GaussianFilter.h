#pragma once
#include "BasePostEffect.h"
/// <summary>
/// ポストエフェクト_GaussianFilter(IPostEffectの派生クラス)
/// </summary>
class GaussianFilter : public BasePostEffect {
public:
	void Finalize() override;
	void Command() override;
private:
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	void RootSignature() override;

	/// <summary>
	/// PixelSharderを作成
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
	D3D12_ROOT_PARAMETER rootParameters_[1] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};

};