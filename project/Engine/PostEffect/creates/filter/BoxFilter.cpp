/// ----------------------
///
/// ボックスフィルター
/// カメラレンズ風ぼかし
/// 
/// ----------------------
#include "BoxFilter.h"
#include <SrvManager.h>

using namespace Logger;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void BoxFilter::Finalize() {}

	void BoxFilter::RootSignature() {

		PostEffectRootSignatureCommon();

		IntroduceRootParameters();
		IntroduceSamplers();
	}

	void BoxFilter::CreatePixelShader() {
		pixelShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/BoxFilter.PS.hlsl", L"ps_6_0");
		assert(pixelShaderBlob != nullptr);
	}

	void BoxFilter::Command() {
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
		DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(3, 1, 0, 0);
	}

	void BoxFilter::EffectInit() {

		srvIndex_ = SrvManager::GetInstance().Allocate();
		srvHandleCPU_ = SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex_);
		srvHandleGPU_ = SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex_);

		SrvManager::GetInstance().CreateSRVForTexture2D(srvIndex_, DirectXCommon::GetInstance().GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);
	}

	void BoxFilter::EffectUpdate() {
#ifdef USE_IMGUI

		ImGui::Text("BoxFilter");
		ImGui::Text("5x5");

#endif
	}
}