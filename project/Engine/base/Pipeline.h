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
	virtual void Initialize() = 0;

	/// <summary>
	/// DescriptorRangeの設定
	/// </summary>
	/// <param name="range">使用するRange</param>
	/// <param name="number">ベースレジスタ番号</param>
	void CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE* range, uint32_t number);

	/// <summary>
	/// コンスタンスバッファ設定
	/// </summary>
	/// <param name="shader">Shader名(VERTEX / PIXEL)</param>
	/// <param name="number">レジスタ番号</param>
	void CreateCBV(D3D12_SHADER_VISIBILITY shader, uint32_t number);

	/// <summary>
	/// テーブル設定
	/// </summary>
	/// <param name="descriptorRange">使用するRange</param>
	void CreateTABLE(D3D12_SHADER_VISIBILITY sharder, D3D12_DESCRIPTOR_RANGE* descriptorRange);
	/// <summary>
	/// 共通のパラメータ
	/// [0] PS b0
	/// [1] VS b0
	/// [2] PS t0
	/// </summary>
	void RootParameterCommon();

	/// <summary>
	/// デフォルト(よく使われる)サンプラー設定
	/// </summary>
	void DefaultSampler(uint32_t arrayNumber);

	/// <summary>
	/// ルートシグネチャにルートパラメータ導入
	/// </summary>
	void IntroduceRootParameters();

	/// <summary>
	/// ルートシグネチャにサンプラー導入
	/// </summary>
	void IntroduceSamplers();

	/// <summary>
	/// エレメントデスク作成
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="format">フォーマット</param>
	void CreateInputElementDesc(const char* name,DXGI_FORMAT format);

	/// <summary>
	/// 共通のエレメントデスク
	/// (POSITION , TEXCORD)
	/// </summary>
	void InputElementDescCommon();

	/// <summary>
	/// 最後にinputLayoutDaceにElementDescsを導入する
	/// </summary>
	void IntroduceInputElementDesc();

protected:
	/// <summary>
	/// グラフィックスパイプライン
	/// </summary>
	void GraphicsPipeline();

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
	/// Rasterizerを作成
	/// </summary>
	virtual void CreateRasterizer() = 0;

	/// <summary>
	/// VertexShaderを作成
	/// </summary>
	virtual void CreateVertexShader() = 0;
	
	/// <summary>
	/// PixelShaderを作成
	/// </summary>
	virtual void CreatePixelShader() = 0;

	/// <summary>
	/// DepthStencilの作成
	/// </summary>
	virtual void CreateDepthStencil() = 0;

	//BlendState
	D3D12_BLEND_DESC blendDesc;

	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	//RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	std::vector<D3D12_ROOT_PARAMETER> rootParameters_ = {};
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_ = {};

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {};

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

	/// <summary>
	/// RootSignatureをバイナリにして
	/// パイプラインの基盤を生成する
	/// </summary>
	void ConvertBinary();

	/// <summary>
	/// Stateにまとめる
	/// </summary>
	void CreateGraphicsPipelineState();

	//InputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

};

