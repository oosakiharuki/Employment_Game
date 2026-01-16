#include "SpriteCommon.h"

using namespace Logger;

std::unique_ptr<SpriteCommon> SpriteCommon::sInstance_ = nullptr;

SpriteCommon& SpriteCommon::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<SpriteCommon>();
	}
	return *sInstance_;
}
void SpriteCommon::Finalize() {
	sInstance_.reset();
}

void SpriteCommon::Initialize() {
	GraphicsPipeline();
}

void SpriteCommon::RootSignature() {

	//RootSignature
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CreateDescriptorRange(descriptorRange_, 0);

	//RootParameter作成__
	RootParameterCommon();//[0,1,2]

	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);//[3] PS b1

	//2でまとめる
	DefaultSampler(0);

	IntroduceRootParameters();
	IntroduceSamplers();
}

void SpriteCommon::CreateInputLayout() {

	InputElementDescCommon();
	CreateInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);

	IntroduceInputElementDesc();
}

void SpriteCommon::CreateBlend() {
	//BlendState
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void SpriteCommon::CreateRasterizer() {
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void SpriteCommon::CreateVertexShader() {
	vertexShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Sprite.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void SpriteCommon::CreatePixelShader() {
	pixelShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Sprite.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void SpriteCommon::CreateDepthStencil() {
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void SpriteCommon::Command() {
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	DirectXCommon::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
