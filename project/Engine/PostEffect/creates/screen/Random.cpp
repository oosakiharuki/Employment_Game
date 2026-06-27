/// ---------------
///
/// ランダム
/// テレビの砂嵐
/// 
/// ---------------
#include "Random.h"
#include <SrvManager.h>

using namespace Logger;
/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void Random::Finalize() {}

	void Random::RootSignature() {

		PostEffectRootSignatureCommon();

		//ランダム
		CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);

		IntroduceRootParameters();
		IntroduceSamplers();
	}

	void Random::EffectInit() {
		srvIndex_ = SrvManager::GetInstance().Allocate();
		srvHandleCPU_ = SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex_);
		srvHandleGPU_ = SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex_);


		SrvManager::GetInstance().CreateSRVForTexture2D(srvIndex_, DirectXCommon::GetInstance().GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

		RandomResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(RandomFunction));
		RandomResource_->Map(0, nullptr, reinterpret_cast<void**>(&randomFunction_));

		randomFunction_->randomTimer = 0.0f;
	}

	void Random::CreatePixelShader() {
		pixelShaderBlob = ShaderManager::GetInstance().CompileShader(L"resource/shaders/Random.PS.hlsl", L"ps_6_0");
		assert(pixelShaderBlob != nullptr);
	}

	void Random::Command() {
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(1, RandomResource_->GetGPUVirtualAddress());
		DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(3, 1, 0, 0);
	}

	void Random::EffectUpdate() {
		randomFunction_->randomTimer += 0.1f;

#ifdef USE_IMGUI
		ImGui::Text("Random");
#endif

	}
}