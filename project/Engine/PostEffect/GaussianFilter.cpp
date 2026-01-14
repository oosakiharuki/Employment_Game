#include "GaussianFilter.h"
#include <SrvManager.h>

using namespace Logger;

void GaussianFilter::Finalize() {}

void GaussianFilter::RootSignature() {
	
	PostEffectRootSignatureCommon();

	//導入する
	IntroduceRootParameters();
	IntroduceSamplers();
}

void GaussianFilter::CreatePixelShader() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/GaussianFilter.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void GaussianFilter::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}


void GaussianFilter::EffectInit() {
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);

	SrvManager::GetInstance()->CreateSRVForTexture2D(srvIndex_, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);
}

void GaussianFilter::EffectUpdate() {

#ifdef USE_IMGUI
	ImGui::Text("GaussianFilter");
	ImGui::Text("9x9");
#endif

}
