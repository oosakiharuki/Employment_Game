#pragma once
#include "DirectXCommon.h"

/// <summary>
/// パイプライン共有処理
/// </summary>
class Pipeline
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon">使用しているDirectXCommon</param>
	virtual void Initialize(DirectXCommon* dxCommon) = 0;


protected:
	/// <summary>
	/// グラフィックスパイプライン
	/// </summary>
	virtual void GraphicsPipeline();

	/// <summary>
	/// ルートシグネチャ
	/// </summary>
	virtual void RootSignature() = 0;

	/// <summary>
	/// InputLayoutを作成
	/// </summary>
	virtual void CreateInputLayout() = 0;

	/// <summary>
	/// Blendを作成
	/// </summary>
	virtual void CreateBlend() = 0;

	/// <summary>
	/// RootSignatureをバイナリにして
	/// パイプラインの基盤を生成する
	/// </summary>
	void ConvertBinary();

	/// <summary>
	/// Rasterizerを作成
	/// </summary>
	virtual void CreateRasterizer() = 0;

	/// <summary>
	/// VertexSharderを作成
	/// </summary>
	virtual void CreateVertexSharder() = 0;
	
	/// <summary>
	/// PixelSharderを作成
	/// </summary>
	virtual void CreatePixelSharder() = 0;

	/// <summary>
	/// DepthStencilの作成
	/// </summary>
	virtual void CreateDepthStencil() = 0;

	/// <summary>
	/// Stateにまとめる
	/// </summary>
	void CreateGraphicsPipelineState();

	//InputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	
	//BlendState
	D3D12_BLEND_DESC blendDesc;

	DirectXCommon* dxCommon_ = nullptr;

	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	//PSO
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_;


	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;


	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

private:

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

};

