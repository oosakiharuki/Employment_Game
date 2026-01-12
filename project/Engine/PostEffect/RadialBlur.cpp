#include "RadialBlur.h"
#include <SrvManager.h>

using namespace Logger;

void RadialBlur::Finalize() {}

void RadialBlur::RootSignature() {
	
	PostEffectRootSignatureCommon();

	IntroduceRootParameters();
	IntroduceSamplers();
}

void RadialBlur::CreatePixelSharder() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/RadialBlur.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void RadialBlur::Command() {

	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void RadialBlur::EffectInit() {
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);

	SrvManager::GetInstance()->CreateSRVforTexture2D(srvIndex_, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);
}

void RadialBlur::EffectUpdate() {
	
#ifdef USE_IMGUI
	ImGui::Text("RadialBlur");
#endif
}
