#include "ParticleCommon.h"

using  namespace Logger;

std::shared_ptr<ParticleCommon> ParticleCommon::sInstance_ = nullptr;

std::shared_ptr<ParticleCommon> ParticleCommon::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<ParticleCommon>();
	}
	return sInstance_;
}
void ParticleCommon::Finalize() {
	sInstance_.reset();
	sInstance_ = nullptr;
}

void ParticleCommon::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

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
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "COLOR";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
}

void ParticleCommon::CreateBlend(){
	//BlendState
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;// srcClor * scrAlpha
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

void ParticleCommon::CreateVertexSharder() {
	//shaderのコンパイラ
	vertexShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void ParticleCommon::CreatePixelSharder() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void ParticleCommon::CreateDepthStencil() {
	//DepthStencilState
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void ParticleCommon::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
