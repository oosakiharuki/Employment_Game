#include "Dissolve.h"
#include <SrvManager.h>
#include <TextureManager.h>
#include <algorithm>

using namespace Logger;

void Dissolve::Finalize() {
	//delete instance;
	//instance = nullptr;
}

void Dissolve::RootSignature() {

	//RootSignature
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	descriptorRange_[0].BaseShaderRegister = 0;
	descriptorRange_[0].NumDescriptors = 1;//t0
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descriptorRangeDissolve_[0].BaseShaderRegister = 1;
	descriptorRangeDissolve_[0].NumDescriptors = 1;//t1
	descriptorRangeDissolve_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeDissolve_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootParameter作成__
	descriptionRootSignature_.pParameters = rootParameters_;
	descriptionRootSignature_.NumParameters = _countof(rootParameters_);


	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[0].DescriptorTable.pDescriptorRanges = descriptorRange_;
	rootParameters_[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters_[1].DescriptorTable.pDescriptorRanges = descriptorRangeDissolve_;
	rootParameters_[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeDissolve_);

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

	descriptionRootSignature_.pStaticSamplers = staticSamplers_;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers_);

}

void Dissolve::CreatePixelSharder() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Dissolve.PS.hlsl", L"ps_6_0");//ココのみ変化させる
	assert(pixelShaderBlob != nullptr);
}

void Dissolve::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	//通常の描画
	
	if (isFade_) {
		//特定のテクスチャ
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureGPU_);
	}
	else {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	}
	
	//Dissolveの描画
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetSrvHandleGPU(textureFileName_));
	//溶ける度合
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, dissolveResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}


void Dissolve::EffectInit() {

	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);


	SrvManager::GetInstance()->CreateSRVforTexture2D(srvIndex_, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

	textureFileName_ = "resource/Sprite/noise0.png";
	TextureManager::GetInstance()->LoadTexture(textureFileName_);

	dissolveResource_ = dxCommon_->CreateBufferResource(sizeof(Threshold));
	dissolveResource_->Map(0, nullptr, reinterpret_cast<void**>(&threshold_));

	threshold_->degress = 0.5f;
	threshold_->edgeSize = 0.02f;
}


void Dissolve::EffectUpdate() {

#ifdef USE_IMGUI

	ImGui::Text("Dissolve");
	ImGui::SliderFloat("溶かし度合", &threshold_->degress, 0.0f, 1.0f);
	ImGui::SliderFloat("egdeのサイズ", &threshold_->edgeSize, 0.0f, 0.1f);
	//ImGui::SliderFloat3("egdeColor", &threshold->egdeColor.x, 0.0f, 1.0f);

	ImGui::Checkbox("マスク画像変更",&isChangeMask_);

	ImGui::Text("現在のマスク画像");
	if (isChangeMask_) {
		ImGui::Text("noise1");
	}
	else {
		ImGui::Text("noise0");
	}
#endif
	//溶かし具合
	if (Input::GetInstance()->PushKey(DIK_D)) {
		threshold_->degress += 0.01f;
	}
	else if (Input::GetInstance()->PushKey(DIK_A)) {
		threshold_->degress -= 0.01f;
	}
	threshold_->degress = std::clamp(threshold_->degress, 0.0f, 1.0f);

	//Edge調節
	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		threshold_->edgeSize += 0.001f;
	}
	else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		threshold_->edgeSize -= 0.001f;
	}
	threshold_->edgeSize = std::clamp(threshold_->edgeSize, 0.0f, 0.1f);

	//マスク変更
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		isChangeMask_ = !isChangeMask_;
	}

	if (isChangeMask_) {
		textureFileName_ = "resource/Sprite/noise1.png";
		TextureManager::GetInstance()->LoadTexture(textureFileName_);
	}
	else {
		textureFileName_ = "resource/Sprite/noise0.png";
		TextureManager::GetInstance()->LoadTexture(textureFileName_);
	}
}

void Dissolve::SetBackGround(D3D12_GPU_DESCRIPTOR_HANDLE gpu,  const std::string& textureFile) {

	TextureGPU_ = gpu;
	isFade_ = true;

	textureFileName_ = "resource/Sprite/" + textureFile;
	TextureManager::GetInstance()->LoadTexture(textureFileName_);
}

void Dissolve::Degress(float value) {
	threshold_->degress = value;
}

void Dissolve::EdgeSize(float value) {
	threshold_->edgeSize = value;
}