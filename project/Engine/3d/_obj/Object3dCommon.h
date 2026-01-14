#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h"

class Camera;
/// <summary>
/// ,objのオブジェクト共有部分
/// </summary>
class Object3dCommon : public Pipeline {
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
	void Initialize(DirectXCommon* dxCommon) override;
	/// <summary>
	/// getter_DirectXCommon
	/// </summary>
	/// <returns></returns>
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
	/// VertexShaderを作成
	/// </summary>
	void CreateVertexShader() override;

	/// <summary>
	/// PixelShaderを作成
	/// </summary>
	void CreatePixelShader() override;

	/// <summary>
	/// DepthStencilの作成
	/// </summary>
	void CreateDepthStencil() override;

	Camera* defaultCamera_ = nullptr;

	static std::shared_ptr<Object3dCommon> sInstance_;
};