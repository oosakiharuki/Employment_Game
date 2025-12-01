#pragma once
#include "DirectXCommon.h"
class Camera;
/// <summary>
/// スキニングの共有
/// </summary>
class SkinningCommon {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<SkinningCommon> GetInstance();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	DirectXCommon* GetDirectXCommon()const { return dxCommon_; }
	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();
	/// <summary>
	/// setter_デフォルトカメラ
	/// </summary>
	/// <param name="camera"></param>現在使っているカメラ
	void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
	/// <summary>
	/// getter_デフォルトカメラ
	/// </summary>
	/// <param name="camera"></param>
	Camera* GetDefaultCamera() const { return defaultCamera_; }

private:
	//PSO
	void RootSignature();
	void GraphicsPipeline();

	DirectXCommon* dxCommon_ = nullptr;

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRangeIBL_[1] = {};//iamge_based_lighting
	D3D12_ROOT_PARAMETER rootParameters_[9] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};



	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	//PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;


	Camera* defaultCamera_ = nullptr;

	static std::shared_ptr<SkinningCommon> sInstance_;
};