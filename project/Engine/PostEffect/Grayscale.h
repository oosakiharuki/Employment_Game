#pragma once
#include "IPostEffects.h"
#include <Vector3.h>

/// <summary>
/// ポストエフェクト_GrayScale(IPostEffectの派生クラス)
/// </summary>
class Grayscale : public IPostEffects {
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
	D3D12_ROOT_PARAMETER rootParameters_[2] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};


	Microsoft::WRL::ComPtr<ID3D12Resource> GrayscaleResource_;
	
	struct GrayFunction {
		int32_t isSepia;
		Vector3 color;
	};

	GrayFunction* grayFunction_;
	bool isSepiaMode_ = false;
};