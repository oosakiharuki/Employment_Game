#include "DebugWireframes.h"

using namespace Logger;

std::unique_ptr<DebugWireframes> DebugWireframes::sInstance_ = nullptr;

DebugWireframes& DebugWireframes::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<DebugWireframes>();
	}
	return *sInstance_;
}
void DebugWireframes::Finalize() {}

void DebugWireframes::Initialize() {
	GraphicsPipeline();
}


void DebugWireframes::RootSignature() {

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

void DebugWireframes::CreateInputLayout() {
	//InputLayout
	InputElementDescCommon();

	CreateInputElementDesc("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	CreateInputElementDesc("WORLDPOSITION", DXGI_FORMAT_R32G32B32_FLOAT);

	IntroduceInputElementDesc();
}

void DebugWireframes::CreateBlend() {
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void DebugWireframes::CreateRasterizer() {
	//RasterizerState
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;//ワイヤーフレーム
}

void DebugWireframes::CreateVertexShader() {
	vertexShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void DebugWireframes::CreatePixelShader() {
	pixelShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Wireframe.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void DebugWireframes::CreateDepthStencil() {
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}


void DebugWireframes::Command() {
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	DirectXCommon::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
