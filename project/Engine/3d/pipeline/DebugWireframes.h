#pragma once
#include "DirectXCommon.h"
#include "Pipeline.h"

class Camera;
/// <summary>
/// ワイヤーフレーム調の共有処理
/// </summary>
class DebugWireframes : public Pipeline {
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static DebugWireframes& GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize();
	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();
	/// <summary>
	/// setter_デフォルトカメラ
	/// </summary>
	/// <param name="camera"></param>
	void SetDefaultCamera(Camera* camera) { defaultCamera_ = camera; }
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
	//インスタンス
	static std::unique_ptr<DebugWireframes> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<DebugWireframes>;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};

};