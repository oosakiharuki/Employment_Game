#include "CubeMap.h"

using namespace Logger;

std::unique_ptr<CubeMap> CubeMap::sInstance_ = nullptr;

CubeMap& CubeMap::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<CubeMap>();
	}
	return *sInstance_;
}
void CubeMap::Finalize() {
	sInstance_.reset();
}

void CubeMap::Initialize() {
	GraphicsPipeline();
}

void CubeMap::RootSignature() {
	//RootSignature
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CreateDescriptorRange(descriptorRange_, 0);

	//RootParameter作成__
	RootParameterCommon();

	DefaultSampler(0);
	
	IntroduceRootParameters();
	IntroduceSamplers();
}

void CubeMap::CreateInputLayout() {
	//InputLayout
	InputElementDescCommon();//POSITION ,TEXCORD
	IntroduceInputElementDesc();//InputLayoutDescに導入する
}

void CubeMap::CreateBlend() {
	//blend
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void CubeMap::CreateRasterizer() {
	//RasterizerState
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void CubeMap::CreateVertexShader() {
	vertexShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Skybox.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void CubeMap::CreatePixelShader() {
	pixelShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Skybox.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void CubeMap::CreateDepthStencil() {
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;//全ピクセルがz=1に出力される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void CubeMap::Command() {
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	DirectXCommon::GetInstance().GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
