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
	void Initialize(DirectXCommon* dxCommon) override;
	/// <summary>
	/// getter_DirectX
	/// </summary>
	/// <returns></returns>
	DirectXCommon* GetDirectXCommon() const { return dxCommon_; }

	/// <summary>
	/// 描画コマンド
	/// </summary>
	void Command();

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


	static std::shared_ptr<SpriteCommon> sInstance_;

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
};