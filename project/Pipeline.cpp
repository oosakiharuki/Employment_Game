#include "Pipeline.h"

using namespace Logger;

void Pipeline::CreateRootSignature() {
	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}


void Pipeline::CreateGraphicsPipelineState() {

	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//PSOここ絶対最後
	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}

void Pipeline::CreateVertexSharder(const std::wstring filePath) {
	vertexShaderBlob = dxCommon_->CompileShader(L"resource/shaders/" + filePath, L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void Pipeline::CreatePixelSharder(const std::wstring filePath) {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/" + filePath, L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Pipeline::CreateRasterizer(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode) {
	rasterizerDesc.CullMode = cullMode;//表裏表示
	rasterizerDesc.FillMode = fillMode;
}

void Pipeline::CreateDepthStencil(bool isDepth, D3D12_DEPTH_WRITE_MASK depthWriteMask, D3D12_COMPARISON_FUNC depthFunc) {
	depthStencilDesc.DepthEnable = isDepth;
	depthStencilDesc.DepthWriteMask = depthWriteMask;
	depthStencilDesc.DepthFunc = depthFunc;
}