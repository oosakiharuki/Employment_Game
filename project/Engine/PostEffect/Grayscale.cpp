#include "Grayscale.h"
#include <SrvManager.h>

using namespace Logger;

void Grayscale::Finalize() {
	//delete instance;
	//instance = nullptr;
}

void Grayscale::RootSignature() {

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

void Grayscale::CreatePixelSharder() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Grayscale.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Grayscale::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, GrayscaleResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Grayscale::EffectInit() {
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);

	SrvManager::GetInstance()->CreateSRVforTexture2D(srvIndex_, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

	GrayscaleResource_ = dxCommon_->CreateBufferResource(sizeof(GrayFunction));
	GrayscaleResource_->Map(0, nullptr, reinterpret_cast<void**>(&grayFunction_));

	grayFunction_->isSepia = false;
	grayFunction_->color = Vector3(1.0f, 74.0f / 107.0f, 43.0f / 107.0f);//セピア調
}

void Grayscale::EffectUpdate() {

#ifdef USE_IMGUI
	ImGui::Text("Grayscale");
	ImGui::Checkbox("セピア調", &isSepiaMode_);
#endif

	//セピア調変更ボタン
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		isSepiaMode_ = !isSepiaMode_;
	}
	if (isSepiaMode_) {
		grayFunction_->isSepia = true;
	}
	else {
		grayFunction_->isSepia = false;
	}
}
