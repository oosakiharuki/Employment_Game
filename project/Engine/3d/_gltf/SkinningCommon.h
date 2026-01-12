#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h"

class Camera;
/// <summary>
/// スキニングの共有
/// </summary>
class SkinningCommon : public Pipeline{
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
	void Initialize(DirectXCommon* dxCommon) override;
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

	static std::shared_ptr<SkinningCommon> sInstance_;
};