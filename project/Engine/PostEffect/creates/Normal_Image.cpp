/// --------------------------------
///
/// 何も手を加えていない状態の画像
/// (通常のレンダーテクスチャ)
/// 
/// --------------------------------
#include "Normal_Image.h"
#include <SrvManager.h>

using namespace Logger;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void Normal_Image::Finalize() {}

	void Normal_Image::RootSignature() {

		//RootSignature
		PostEffectRootSignatureCommon();

		IntroduceRootParameters();
		IntroduceSamplers();
	}

	void Normal_Image::CreatePixelShader() {
		pixelShaderBlob = ShaderManager::GetInstance().CompileShader(L"resource/shaders/CopyImage.PS.hlsl", L"ps_6_0");
		assert(pixelShaderBlob != nullptr);
	}

	void Normal_Image::Command() {
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
		DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(3, 1, 0, 0);
	}

	void Normal_Image::EffectInit() {
		srvIndex_ = SrvManager::GetInstance().Allocate();
		srvHandleCPU_ = SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex_);
		srvHandleGPU_ = SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex_);

		SrvManager::GetInstance().CreateSRVForTexture2D(srvIndex_, DirectXCommon::GetInstance().GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);
	}

	void Normal_Image::EffectUpdate() {

#ifdef USE_IMGUI
		ImGui::Text("Normal_Image(copyImage)");
#endif

	}
}