#include "DepthBasedOutline.h"
#include <SrvManager.h>

using namespace Logger;
using namespace MyMath;

void DepthBasedOutline::Finalize() {
	//delete instance;
	//instance = nullptr;
}

void DepthBasedOutline::RootSignature() {

	//RootSignature
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	descriptorRange_[0].BaseShaderRegister = 0;
	descriptorRange_[0].NumDescriptors = 1;//t0
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descriptorRangeOutline_[0].BaseShaderRegister = 1;
	descriptorRangeOutline_[0].NumDescriptors = 1;//t1
	descriptorRangeOutline_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeOutline_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootParameter作成__
	descriptionRootSignature_.pParameters = rootParameters_;
	descriptionRootSignature_.NumParameters = _countof(rootParameters_);


	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[0].DescriptorTable.pDescriptorRanges = descriptorRange_;
	rootParameters_[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[1].DescriptorTable.pDescriptorRanges = descriptorRangeOutline_;
	rootParameters_[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeOutline_);


	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[2].Descriptor.ShaderRegister = 0;//Object3d.PS.hlsl の b0


	//2でまとめる
	//Sampler s0
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//clamp = そのテクスチャが伸びる
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers_[0].ShaderRegister = 0;
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//SamplerPoint s1
	staticSamplers_[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	staticSamplers_[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//clamp = そのテクスチャが伸びる
	staticSamplers_[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers_[1].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers_[1].ShaderRegister = 1;
	staticSamplers_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature_.pStaticSamplers = staticSamplers_;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers_);

}

void DepthBasedOutline::CreateInputLayout() {
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = nullptr;
	inputLayoutDesc.NumElements = 0;
}

void DepthBasedOutline::CreateBlend() {
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void DepthBasedOutline::CreateRasterizer() {
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void DepthBasedOutline::CreateVertexSharder() {
	vertexShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Fullscreen.VS.hlsl", L"vs_6_0");//フルスクリーン処理(共通処理)
	assert(vertexShaderBlob != nullptr);
}

void DepthBasedOutline::CreatePixelSharder() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/DepthBasedOutline.PS.hlsl", L"ps_6_0");//ココのみ変化させる
	assert(pixelShaderBlob != nullptr);
}

void DepthBasedOutline::CreateDepthStencil() {
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void DepthBasedOutline::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	//通常の描画
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	//outlineの描画
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvHandleGPUDepth_);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void DepthBasedOutline::EffectInit() {
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);

	SrvManager::GetInstance()->CreateSRVforTexture2D(srvIndex_, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPUDepth_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPUDepth_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);

	SrvManager::GetInstance()->CreateSRVforTexture2D(srvIndex_, dxCommon_->GetOutlineResource(), DXGI_FORMAT_R24_UNORM_X8_TYPELESS, 1);

	//Model用マテリアル
	//マテリアル用のリソース
	materialResource_ = dxCommon_->CreateBufferResource(sizeof(DepthOutlineFunction));
	//書き込むためのアドレス
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&depthOutlineFunction_));
	//色の設定
	depthOutlineFunction_->projectionInverse = 150.0f;
}

void DepthBasedOutline::EffectUpdate() {

#ifdef USE_IMGUI
	ImGui::Text("DepthBasedOutline");
	//ImGui::SliderFloat("線", &depthOutlineFunction->projectionInverse, 0.0f, 100.0f);
#endif

}
