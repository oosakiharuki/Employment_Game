#pragma once
#include "DirectXCommon.h"
class Camera;
/// <summary>
/// ,objのオブジェクト共有部分
/// </summary>
class Object3dCommon {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<Object3dCommon> GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	DirectXCommon* GetDirectXCommon()const { return dxCommon_; }
	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();	
	/// <summary>
	/// setter_デフォルトカメラ
	/// </summary>
	/// <param name="camera"></param> 現在使用しているカメラ
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
	D3D12_ROOT_PARAMETER rootParameters_[7] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};


	//バイナリを元に生成
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature_ = nullptr;

	//PSO
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	Camera* defaultCamera_ = nullptr;

	static std::shared_ptr<Object3dCommon> sInstance_;
};