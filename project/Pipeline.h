#pragma once
#include "DirectXCommon.h"

/// <summary>
/// パイプライン共有処理
/// </summary>
class Pipeline
{
public:
	/// <summary>
	/// InputLayoutを作成
	/// </summary>
	virtual void CreateInputLayout() = 0;

	/// <summary>
	/// Blendを作成
	/// </summary>
	virtual void CreateBlend() = 0;

	/// <summary>
	/// RootSignatureのバイナリにして生成する
	/// </summary>
	void CreateRootSignature();


	/// <summary>
	/// VertexSharderを作成
	/// </summary>
	/// <param name="filePath">vs.hslsのfilePath</param>
	void CreateVertexSharder(const std::wstring filePath);
	
	/// <summary>
	/// PixelSharderを作成
	/// </summary>
	/// <param name="filePath">ps.hslsのfilePath</param>
	void CreatePixelSharder(const std::wstring filePath);

	/// <summary>
	/// Rasterizerを作成
	/// </summary>
	/// <param name="cullMode">表示面の設定(裏{BACK} / 表{FRONT} / 両方{NONE})</param>
	/// <param name="fillMode">描画モード(SOLID / WIREFRAME)</param>
	void CreateRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);

	/// <summary>
	/// DepthStencilの作成
	/// </summary>
	/// <param name="isDepth">使うか使わないか</param>
	/// <param name="depthWriteMask">書き込み</param>
	/// <param name="depthFunc"></param>
	void CreateDepthStencil(bool isDepth ,D3D12_DEPTH_WRITE_MASK depthWriteMask , D3D12_COMPARISON_FUNC depthFunc);

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


private:

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;


	D3D12_RASTERIZER_DESC rasterizerDesc{};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	//DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

};

