#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h"

class Camera;
/// <summary>
/// .gltf版のオブジェクト共有部分
/// </summary>
class GLTFCommon : public Pipeline{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<GLTFCommon> GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(DirectXCommon* dxCommon) override;
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

	//RootSignature
	D3D12_DESCRIPTOR_RANGE descriptorRangeIBL_[1] = {};//iamge_based_lighting

	Camera* defaultCamera_ = nullptr;

	static std::shared_ptr<GLTFCommon> sInstance_;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
};