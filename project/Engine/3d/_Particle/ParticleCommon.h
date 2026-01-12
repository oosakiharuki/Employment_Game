#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h"

class Camera;
/// <summary>
/// パーティクルの共有部分
/// </summary>
class ParticleCommon : public Pipeline {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<ParticleCommon> GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>使用しているdirectXCommon
	void Initialize(DirectXCommon* dxCommon) override;
	DirectXCommon* GetDxCommon()const { return dxCommon_; }

	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();
	/// <summary>
	/// setter_デフォルトカメラ
	/// </summary>
	/// <param name="camera"></param>使用するカメラ
	void SetDefaultCamera(Camera* camera) { defaultCamera_ = camera; }
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
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing_[1] = {};

	Camera* defaultCamera_ = nullptr;


	static std::shared_ptr<ParticleCommon> sInstance_;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
};