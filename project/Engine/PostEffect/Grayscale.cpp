#include "Grayscale.h"
#include <SrvManager.h>

using namespace Logger;

void Grayscale::Finalize() {}

void Grayscale::RootSignature() {

	PostEffectRootSignatureCommon();

	//セピア調
	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);//[1] ps b0

	IntroduceRootParameters();
	IntroduceSamplers();
}

void Grayscale::CreatePixelShader() {
	pixelShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Grayscale.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
}

void Grayscale::Command() {
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(1, GrayscaleResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void Grayscale::EffectInit() {
	srvIndex_ = SrvManager::GetInstance().Allocate();
	srvHandleCPU_ = SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex_);

	SrvManager::GetInstance().CreateSRVForTexture2D(srvIndex_, DirectXCommon::GetInstance().GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

	GrayscaleResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(GrayFunction));
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
	if (Input::GetInstance().TriggerKey(DIK_RETURN)) {
		isSepiaMode_ = !isSepiaMode_;
	}
	if (isSepiaMode_) {
		grayFunction_->isSepia = true;
	}
	else {
		grayFunction_->isSepia = false;
	}
}
