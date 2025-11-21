#pragma once
#include "DirectXCommon.h"
/// <summary>
/// スプライトの共有部分
/// </summary>
class SpriteCommon {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static SpriteCommon* GetInstance();
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
	DirectXCommon* GetDirectXCommon()const { return dxCommon_; }

	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();

private:
	//PSO
	void RootSignature();
	void GraphicsPipeline();


	DirectXCommon* dxCommon_;

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[4] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};


	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	//PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;


	static SpriteCommon* sInstance_;

	SpriteCommon() = default;
	~SpriteCommon() = default;
	SpriteCommon(SpriteCommon&) = default;
	SpriteCommon& operator=(SpriteCommon&) = default;

	static uint32_t sSRVIndexTop_;
};