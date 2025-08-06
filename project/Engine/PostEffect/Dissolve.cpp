#include "Dissolve.h"
#include <SrvManager.h>
#include <TextureManager.h>
#include <algorithm>

using namespace Logger;

void Dissolve::Finalize() {
	//delete instance;
	//instance = nullptr;
}

void Dissolve::Initialize(DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	GraphicsPipeline();
}

void Dissolve::RootSignature() {

	//RootSignature
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;//t0
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descriptorRangeDissolve[0].BaseShaderRegister = 1;
	descriptorRangeDissolve[0].NumDescriptors = 1;//t1
	descriptorRangeDissolve[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeDissolve[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootParameter作成__
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);


	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeDissolve;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeDissolve);

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].Descriptor.ShaderRegister = 0;//Object3d.PS.hlsl の b0


	//2でまとめる
	//Sampler s0
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//clamp = そのテクスチャが伸びる
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

}

void Dissolve::GraphicsPipeline() {
	RootSignature();

	//バイナリを元に生成

	//シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;


	//BlendState
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	//RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//shaderのコンパイラ
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Fullscreen.VS.hlsl", L"vs_6_0");//フルスクリーン処理(共通処理)
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Dissolve.PS.hlsl", L"ps_6_0");//ココのみ変化させる
	assert(pixelShaderBlob != nullptr);


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
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


	//DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	//PSOここ絶対最後

	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

	srvIndex = SrvManager::GetInstance()->Allocate();
	srvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex);
	srvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex);


	SrvManager::GetInstance()->CreateSRVforTexture2D(srvIndex, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

	textureFileName = "resource/Sprite/noise0.png";
	TextureManager::GetInstance()->LoadTexture(textureFileName);

	dissolveResource = dxCommon_->CreateBufferResource(sizeof(Threshold));
	dissolveResource->Map(0, nullptr, reinterpret_cast<void**>(&threshold));

	threshold->degress = 0.5f;
	threshold->egdeSize = 0.03f;
	//threshold->egdeColor = { 1.0f, 0.4f, 0.3f, 1.0f };//なぜか反映できない
}

void Dissolve::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
	//通常の描画
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU);
	//Dissolveの描画
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetSrvHandleGPU(textureFileName));
	//溶ける度合
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, dissolveResource->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Dissolve::EffectUpdate() {

#ifdef _DEBUG

	ImGui::Text("Dissolve");
	ImGui::SliderFloat("溶かし度合", &threshold->degress, 0.0f, 1.0f);
	ImGui::SliderFloat("egdeのサイズ", &threshold->egdeSize, 0.0f, 0.1f);
	//ImGui::SliderFloat3("egdeColor", &threshold->egdeColor.x, 0.0f, 1.0f);

	ImGui::Checkbox("マスク画像変更",&isChangeMask);

	ImGui::Text("現在のマスク画像");
	if (isChangeMask) {
		ImGui::Text("noise1");
	}
	else {
		ImGui::Text("noise0");
	}
#endif
	//溶かし具合
	if (Input::GetInstance()->PushKey(DIK_D)) {
		threshold->degress += 0.01f;
	}
	else if (Input::GetInstance()->PushKey(DIK_A)) {
		threshold->degress -= 0.01f;
	}
	threshold->degress = std::clamp(threshold->degress, 0.0f, 1.0f);

	//Edge調節
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		threshold->egdeSize += 0.001f;
	}
	else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		threshold->egdeSize -= 0.001f;
	}
	threshold->egdeSize = std::clamp(threshold->egdeSize, 0.0f, 0.1f);

	//マスク変更
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		isChangeMask = !isChangeMask;
	}

	if (isChangeMask) {
		textureFileName = "resource/Sprite/noise1.png";
		TextureManager::GetInstance()->LoadTexture(textureFileName);
	}
	else {
		textureFileName = "resource/Sprite/noise0.png";
		TextureManager::GetInstance()->LoadTexture(textureFileName);
	}
}
