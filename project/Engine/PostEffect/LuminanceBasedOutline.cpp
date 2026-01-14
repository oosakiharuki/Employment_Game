#include "LuminanceBasedOutline.h"
#include <SrvManager.h>

using namespace Logger;

void LuminanceBasedOutline::Finalize() {}

void LuminanceBasedOutline::RootSignature() {

	PostEffectRootSignatureCommon();

	IntroduceRootParameters();
	IntroduceSamplers();
}

void LuminanceBasedOutline::CreatePixelShader() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/LuminanceBasedOutline.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void LuminanceBasedOutline::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void LuminanceBasedOutline::EffectInit() {
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);


	SrvManager::GetInstance()->CreateSRVForTexture2D(srvIndex_, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

}

void LuminanceBasedOutline::EffectUpdate() {

#ifdef USE_IMGUI
	ImGui::Text("LuminanceBasedOutline");
#endif

}
