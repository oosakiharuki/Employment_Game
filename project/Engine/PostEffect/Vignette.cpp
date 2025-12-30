#include "Vignette.h"
#include <SrvManager.h>
#include <algorithm>

using namespace Logger;

void Vignette::Finalize() {
	//delete instance;
	//instance = nullptr;
}

void Vignette::RootSignature() {

	//RootSignature
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptorRange_[0].BaseShaderRegister = 0;
	descriptorRange_[0].NumDescriptors = 1;
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//RootParameter作成__
	descriptionRootSignature_.pParameters = rootParameters_;
	descriptionRootSignature_.NumParameters = _countof(rootParameters_);


	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[0].DescriptorTable.pDescriptorRanges = descriptorRange_;
	rootParameters_[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);
	
	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[1].Descriptor.ShaderRegister = 0;

	//2でまとめる
	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//clamp = そのテクスチャが伸びる
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers_[0].ShaderRegister = 0;
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers_;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers_);

}

void Vignette::CreatePixelSharder() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Vignette.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Vignette::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, vignetteResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Vignette::EffectInit() {
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);


	SrvManager::GetInstance()->CreateSRVforTexture2D(srvIndex_, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);


	vignetteResource_ = dxCommon_->CreateBufferResource(sizeof(VignetteFunction));
	vignetteResource_->Map(0, nullptr, reinterpret_cast<void**>(&vignetteFunction_));

	vignetteFunction_->luminance = 16;
	vignetteFunction_->darkness = 0.8f;
}

void Vignette::EffectUpdate() {

#ifdef USE_IMGUI
	ImGui::Text("Vignette");
	ImGui::SliderFloat("明るさ", &vignetteFunction_->luminance, 0.0f, 100.0f);
	ImGui::SliderFloat("暗さ", &vignetteFunction_->darkness, 0.0f, 2.0f);
#endif

	//明るさの調節
	if (Input::GetInstance()->PushKey(DIK_D)) {
		vignetteFunction_->luminance += 1.0f;
	}
	else if (Input::GetInstance()->PushKey(DIK_A)) {
		vignetteFunction_->luminance -= 1.0f;
	}
	vignetteFunction_->luminance = std::clamp(vignetteFunction_->luminance, 0.0f, 100.0f);

	//明るさの調節
	if (Input::GetInstance()->PushKey(DIK_RIGHT) && vignetteFunction_->darkness < 2.0f) {
		vignetteFunction_->darkness += 0.01f;
	}
	else if (Input::GetInstance()->PushKey(DIK_LEFT) && vignetteFunction_->darkness > 0.0f) {
		vignetteFunction_->darkness -= 0.01f;
	}
	vignetteFunction_->darkness = std::clamp(vignetteFunction_->darkness, 0.0f, 2.0f);

}
