/// --------------------------------------------
///
/// ビネット
/// 周りが暗くなる(過去の回想シーンのイメージ)
/// 
/// --------------------------------------------
#include "Vignette.h"
#include <SrvManager.h>
#include <algorithm>

using namespace Logger;
/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void Vignette::Finalize() {}

	void Vignette::RootSignature() {

		PostEffectRootSignatureCommon();

		CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		IntroduceRootParameters();
		IntroduceSamplers();
	}

	void Vignette::CreatePixelShader() {
		pixelShaderBlob = ShaderManager::GetInstance().CompileShader(L"resource/shaders/Vignette.PS.hlsl", L"ps_6_0");
		assert(pixelShaderBlob != nullptr);
	}

	void Vignette::Command() {
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(1, vignetteResource_->GetGPUVirtualAddress());
		DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(3, 1, 0, 0);
	}

	void Vignette::EffectInit() {
		srvIndex_ = SrvManager::GetInstance().Allocate();
		srvHandleCPU_ = SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex_);
		srvHandleGPU_ = SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex_);


		SrvManager::GetInstance().CreateSRVForTexture2D(srvIndex_, DirectXCommon::GetInstance().GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);


		vignetteResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(VignetteFunction));
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
		if (Input::GetInstance().PushKey(DIK_D)) {
			vignetteFunction_->luminance += 1.0f;
		}
		else if (Input::GetInstance().PushKey(DIK_A)) {
			vignetteFunction_->luminance -= 1.0f;
		}
		vignetteFunction_->luminance = std::clamp(vignetteFunction_->luminance, 0.0f, 100.0f);

		//明るさの調節
		if (Input::GetInstance().PushKey(DIK_RIGHT) && vignetteFunction_->darkness < 2.0f) {
			vignetteFunction_->darkness += 0.01f;
		}
		else if (Input::GetInstance().PushKey(DIK_LEFT) && vignetteFunction_->darkness > 0.0f) {
			vignetteFunction_->darkness -= 0.01f;
		}
		vignetteFunction_->darkness = std::clamp(vignetteFunction_->darkness, 0.0f, 2.0f);

	}
}