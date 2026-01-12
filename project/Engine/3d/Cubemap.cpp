#include "Cubemap.h"

using namespace Logger;

std::shared_ptr<Cubemap> Cubemap::sInstance_ = nullptr;

std::shared_ptr<Cubemap> Cubemap::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<Cubemap>();
	}
	return sInstance_;
}
void Cubemap::Finalize() {
	sInstance_.reset();
	sInstance_ = nullptr;
}
void Cubemap::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	GraphicsPipeline();
}

void Cubemap::RootSignature() {
	//RootSignature
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CreateDescriptorRange(descriptorRange_, 0);

	//RootParameter作成__
	RootParameterCommon();

	DefaultSampler(0);
	
	IntroduceRootParameters();
	IntroduceSamplers();
}

void Cubemap::CreateInputLayout() {
	//InputLayout
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
}

void Cubemap::CreateBlend() {
	//blend
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void Cubemap::CreateRasterizer() {
	//RasterizerState
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void Cubemap::CreateVertexSharder() {
	vertexShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Skybox.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void Cubemap::CreatePixelSharder() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Skybox.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Cubemap::CreateDepthStencil() {
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;//全ピクセルがz=1に出力される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void Cubemap::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
