#include "ParticleCommon.h"

using  namespace Logger;

std::unique_ptr<ParticleCommon> ParticleCommon::sInstance_ = nullptr;

ParticleCommon& ParticleCommon::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<ParticleCommon>();
	}
	return *sInstance_;
}
void ParticleCommon::Finalize() {
	sInstance_.reset();
}

void ParticleCommon::Initialize() {
	GraphicsPipeline();
}

void ParticleCommon::RootSignature() {

	//RootSignature
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CreateDescriptorRange(descriptorRangeForInstancing_, 0);//パーテイクル
	CreateDescriptorRange(descriptorRange_, 0);

	//RootParameter作成__
	RootParameterCommon();

	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);//[3] ps b1
	CreateTABLE(D3D12_SHADER_VISIBILITY_VERTEX,descriptorRangeForInstancing_);//[4] vs t1
	rootParameters_[4].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing_);

	//2でまとめる
	DefaultSampler(0);
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//パーティクルはココだけ違う

	IntroduceRootParameters();
	IntroduceSamplers();
}

void ParticleCommon::CreateInputLayout() {
	//InputLayout
	InputElementDescCommon();
	CreateInputElementDesc("COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT);

	IntroduceInputElementDesc();
}

void ParticleCommon::CreateBlend(){
	//BlendState
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;// srcColor * scrAlpha
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // + 
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;// DestColor * (1-SrcAlpha)

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void ParticleCommon::CreateRasterizer(){
	//RasterizerState

	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void ParticleCommon::CreateVertexShader() {
	//shaderのコンパイラ
	vertexShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void ParticleCommon::CreatePixelShader() {
	pixelShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void ParticleCommon::CreateDepthStencil() {
	//DepthStencilState
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void ParticleCommon::Command() {
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	DirectXCommon::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
