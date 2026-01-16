#include "Object3dCommon.h"

using namespace Logger;

std::unique_ptr<Object3dCommon> Object3dCommon::sInstance_ = nullptr;

Object3dCommon& Object3dCommon::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<Object3dCommon>();
	}
	return *sInstance_;
}
void Object3dCommon::Finalize() {}

void Object3dCommon::Initialize() {
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
	InputElementDescCommon();
	CreateInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	CreateInputElementDesc("WORLDPOSITION", DXGI_FORMAT_R32G32B32_FLOAT);

	IntroduceInputElementDesc();
}

void Object3dCommon::CreateBlend() {
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void Object3dCommon::CreateRasterizer() {
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void Object3dCommon::CreateVertexShader() {
	vertexShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void Object3dCommon::CreatePixelShader() {
	pixelShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Object3dCommon::CreateDepthStencil() {
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void Object3dCommon::Command() {
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	DirectXCommon::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
