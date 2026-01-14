#include "Random.h"
#include <SrvManager.h>

using namespace Logger;

void Random::Finalize() {}

void Random::RootSignature() {

	PostEffectRootSignatureCommon();

	//ランダム
	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);

	IntroduceRootParameters();
	IntroduceSamplers();
}

void Random::EffectInit() {
	srvIndex_ = SrvManager::GetInstance()->Allocate();
	srvHandleCPU_ = SrvManager::GetInstance()->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_);


	SrvManager::GetInstance()->CreateSRVForTexture2D(srvIndex_, dxCommon_->GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

	RandomResource_ = dxCommon_->CreateBufferResource(sizeof(RandomFunction));
	RandomResource_->Map(0, nullptr, reinterpret_cast<void**>(&randomFunction_));

	randomFunction_->randomTimer = 0.0f;
}

void Random::CreatePixelShader() {
	pixelShaderBlob = dxCommon_->CompileShader(L"resource/shaders/Random.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Random::Command() {
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, RandomResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Random::EffectUpdate() {
	randomFunction_->randomTimer += 0.1f;

#ifdef USE_IMGUI
	ImGui::Text("Random");
#endif

}
