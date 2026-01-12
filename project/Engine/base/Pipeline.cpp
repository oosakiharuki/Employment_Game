#include "Pipeline.h"

using namespace Logger;

void Pipeline::GraphicsPipeline() {
	//ルートシグネチャ
	RootSignature();

	//パイプライン最初の処理
	ConvertBinary();

	//InputLayout
	CreateInputLayout();

	//BlendState
	CreateBlend();

	//RasterizerState
	CreateRasterizer();

	//shaderのコンパイラ
	CreateVertexSharder();
	CreatePixelSharder();

	//DepthStencilState
	CreateDepthStencil();

	//PSOここ絶対最後
	CreateGraphicsPipelineState();
}

void Pipeline::ConvertBinary() {
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

void Pipeline::CreateDescriptorRange(D3D12_DESCRIPTOR_RANGE* range, uint32_t number) {
	range[0].BaseShaderRegister = number;
	range[0].NumDescriptors = 1;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void Pipeline::CreateCBV(D3D12_SHADER_VISIBILITY sharder, uint32_t number) {
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter.ShaderVisibility = sharder;
	rootParameter.Descriptor.ShaderRegister = number;//b0,b1など

	rootParameters_.push_back(rootParameter);
}

void Pipeline::CreateTABLE(D3D12_SHADER_VISIBILITY sharder, D3D12_DESCRIPTOR_RANGE* descriptorRange) {
	D3D12_ROOT_PARAMETER rootParameter{};
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter.ShaderVisibility = sharder;
	rootParameter.DescriptorTable.pDescriptorRanges = descriptorRange;

	rootParameters_.push_back(rootParameter);
}

void Pipeline::RootParameterCommon() {
	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);//[0] ps b0
	CreateCBV(D3D12_SHADER_VISIBILITY_VERTEX, 0);//[1] vs b0

	CreateTABLE(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRange_);//[2] ps t0
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);
}

void Pipeline::DefaultSampler(uint32_t arrayNumber) {
	D3D12_STATIC_SAMPLER_DESC staticSampler{};
	staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
	staticSampler.ShaderRegister = arrayNumber;
	staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	staticSamplers_.push_back(staticSampler);
}

void Pipeline::IntroduceRootParameters() {
	//ルートパラメータ導入
	descriptionRootSignature_.pParameters = rootParameters_.data();
	descriptionRootSignature_.NumParameters = UINT(rootParameters_.size());
}

void Pipeline::IntroduceSamplers() {
	//サンプラー導入
	descriptionRootSignature_.pStaticSamplers = staticSamplers_.data();
	descriptionRootSignature_.NumStaticSamplers = UINT(staticSamplers_.size());
}

void Pipeline::CreateInputElementDesc(const char* name, DXGI_FORMAT format) {
	D3D12_INPUT_ELEMENT_DESC inputElementDesc{};
	inputElementDesc.SemanticName = name;
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = format;
	inputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs.push_back(inputElementDesc);
}

void Pipeline::InputElementDeceCommon() {
	CreateInputElementDesc("POSITION", DXGI_FORMAT_R32G32B32A32_FLOAT);//座標
	CreateInputElementDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT);//テクスチャ
}


void Pipeline::IntroduceInputElementDesc() {
	//インプットエレメントデスク導入
	inputLayoutDesc.pInputElementDescs = inputElementDescs.data();
	inputLayoutDesc.NumElements = UINT(inputElementDescs.size());
}
