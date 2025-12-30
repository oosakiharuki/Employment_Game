#pragma once
#include "IPostEffects.h"
#include "MyMath.h"
/// <summary>
/// ポストエフェクト_DepthBasedOutline(IPostEffectの派生クラス)
/// </summary>
class DepthBasedOutline : public IPostEffects {
public:
	void Finalize() override;
	void Command() override;
private:
	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	void RootSignature() override;

	/// <summary>
	/// InputLayoutを作成
	/// </summary>
	void CreateInputLayout() override;

	/// <summary>
	/// Blendを作成
	/// </summary>
	void CreateBlend() override;

	/// <summary>
	/// Rasterizerを作成
	/// </summary>
	void CreateRasterizer() override;

	/// <summary>
	/// VertexSharderを作成
	/// </summary>
	void CreateVertexSharder() override;

	/// <summary>
	/// PixelSharderを作成
	/// </summary>
	void CreatePixelSharder() override;

	/// <summary>
	/// DepthStencilの作成
	/// </summary>
	void CreateDepthStencil() override;

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
	D3D12_DESCRIPTOR_RANGE descriptorRangeOutline_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[3] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[2] = {};

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	struct DepthOutlineFunction {
		float projectionInverse;
	};
	
	DepthOutlineFunction* depthOutlineFunction_ = nullptr;

};