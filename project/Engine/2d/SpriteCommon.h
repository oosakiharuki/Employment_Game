#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h" 

/// <summary>
/// スプライトの共有部分
/// </summary>
class SpriteCommon : public Pipeline{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<SpriteCommon> GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// getter_DirectX
	/// </summary>
	/// <returns></returns>
	DirectXCommon* GetDirectXCommon() const { return dxCommon_; }

	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();

	void CreateInputLayout() override;

	void CreateBlend() override;

private:
	//PSO
	void RootSignature();
	void GraphicsPipeline();

	//RootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[4] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};


	static std::shared_ptr<SpriteCommon> sInstance_;

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
};