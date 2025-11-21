#pragma once
#include "DirectXCommon.h"
class Camera;
/// <summary>
/// .gltf版のオブジェクト共有部分
/// </summary>
class GLTFCommon {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static GLTFCommon* GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();	
	/// <summary>
	/// getter_dirextX
	/// </summary>
	/// <returns></returns>
	DirectXCommon* GetDirectXCommon()const { return dxCommon_; }

	/// <summary>
	/// setter_デフォルトカメラ
	/// </summary>
	/// <returns></returns> 現在使っているカメラ
	void SetDefaultCamera(Camera* camera) { this->defaultCamera_ = camera; }
	/// <summary>
	/// getter_デフォルトカメラ
	/// </summary>
	/// <returns></returns>
	Camera* GetDefaultCamera() const { return defaultCamera_; }
private:
	//PSO
	void RootSignature();
	void GraphicsPipeline();


	DirectXCommon* dxCommon_;

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRangeIBL_[1] = {};//iamge_based_lighting
	D3D12_ROOT_PARAMETER rootParameters_[8] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};


	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	//PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	Camera* defaultCamera_ = nullptr;

	static GLTFCommon* sInstance_;

	GLTFCommon() = default;
	~GLTFCommon() = default;
	GLTFCommon(GLTFCommon&) = default;
	GLTFCommon& operator=(GLTFCommon&) = default;


	static uint32_t sSRVIndexTop_;
};