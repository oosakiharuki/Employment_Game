#pragma once
#include "IPostEffects.h"
/// <summary>
/// ポストエフェクト_通常、何もなし(IPostEffectの派生クラス)
/// </summary>
class Normal_Image : public IPostEffects {
public:
	void Finalize() override;
	void Command() override;
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
	D3D12_ROOT_PARAMETER rootParameters_[1] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
};