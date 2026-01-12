#include "Object3dCommon.h"

using namespace Logger;

std::shared_ptr<Object3dCommon> Object3dCommon::sInstance_ = nullptr;

std::shared_ptr<Object3dCommon> Object3dCommon::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<Object3dCommon>();
	}
	return sInstance_;
}
void Object3dCommon::Finalize() {
	sInstance_.reset();
	sInstance_ = nullptr;
}

void Object3dCommon::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	GraphicsPipeline();
}

void Object3dCommon::RootSignature() {

	//RootSignature
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CreateDescriptorRange(descriptorRange_, 0);

	//RootParameter作成__
	RootParameterCommon();

	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 1);//[3] ps b1
	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 2);//[4] ps b2
	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 3);//[5] ps b3
	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 4);//[6] ps b4

	//2でまとめる

	DefaultSampler(0);//s1

	IntroduceRootParameters();
	IntroduceSamplers();
}

void Object3dCommon::CreateInputLayout() {
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[3].SemanticName = "WORLDPOSITION";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
}

void Object3dCommon::CreateBlend() {
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void Object3dCommon::CreateRasterizer() {
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void Object3dCommon::CreateVertexSharder() {
	vertexShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void Object3dCommon::CreatePixelSharder() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Object3dCommon::CreateDepthStencil() {
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void Object3dCommon::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
