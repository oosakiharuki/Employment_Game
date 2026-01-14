#pragma once
#include "BasePostEffect.h"
#include "MyMath.h"
/// <summary>
/// ポストエフェクト_DepthBasedOutline(IPostEffectの派生クラス)
/// </summary>
class DepthBasedOutline : public BasePostEffect {
public:
	void Finalize() override;
	void Command() override;
private:
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	void RootSignature() override;

	/// <summary>
	/// Blendを作成
	/// </summary>
	void CreateBlend() override;

	/// <summary>
	/// Rasterizerを作成
	/// </summary>
	void CreateRasterizer() override;

	/// <summary>
	/// PixelShaderを作成
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


	//RootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRangeOutline_[1] = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	struct DepthOutlineFunction {
		float projectionInverse;
	};
	
	DepthOutlineFunction* depthOutlineFunction_ = nullptr;

};