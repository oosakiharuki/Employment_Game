/// ------------
///
/// ディゾルブ
/// 溶ける
/// 
/// ------------
#include "Dissolve.h"
#include <SrvManager.h>
#include <TextureManager.h>
#include <algorithm>

using namespace Logger;
/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	void Dissolve::Finalize() {}

	void Dissolve::RootSignature() {

		PostEffectRootSignatureCommon();

		CreateDescriptorRange(descriptorRangeDissolve_, 1);//t1用

		CreateTABLE(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRangeDissolve_);//[1] ps t1
		rootParameters_[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeDissolve_);

		CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);//[2] ps b1

		IntroduceRootParameters();
		IntroduceSamplers();
	}

	void Dissolve::CreatePixelShader() {
		pixelShaderBlob = ShaderManager::GetInstance().CompileShader(L"resource/shaders/Dissolve.PS.hlsl", L"ps_6_0");//ココのみ変化させる
		assert(pixelShaderBlob != nullptr);
	}

	void Dissolve::Command() {
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		//通常の描画

		if (isFade_) {
			//特定のテクスチャ
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, TextureGPU_);
		}
		else {
			DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
		}

		//Dissolveの描画
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance().GetSrvHandleGPU(textureFileName_));
		//溶ける度合
		DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, dissolveResource_->GetGPUVirtualAddress());
		DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(3, 1, 0, 0);
	}


	void Dissolve::EffectInit() {

		srvIndex_ = SrvManager::GetInstance().Allocate();
		srvHandleCPU_ = SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex_);
		srvHandleGPU_ = SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex_);


		SrvManager::GetInstance().CreateSRVForTexture2D(srvIndex_, DirectXCommon::GetInstance().GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

		textureFileName_ = "resource/Sprite/noise0.png";
		TextureManager::GetInstance().LoadTexture(textureFileName_);

		dissolveResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(Threshold));
		dissolveResource_->Map(0, nullptr, reinterpret_cast<void**>(&threshold_));

		threshold_->degree = 0.5f;
		threshold_->edgeSize = 0.02f;
	}


	void Dissolve::EffectUpdate() {

#ifdef USE_IMGUI

		ImGui::Text("Dissolve");
		ImGui::SliderFloat("溶かし度合", &threshold_->degree, 0.0f, 1.0f);
		ImGui::SliderFloat("egdeのサイズ", &threshold_->edgeSize, 0.0f, 0.1f);

		ImGui::Checkbox("マスク画像変更", &isChangeMask_);

		ImGui::Text("現在のマスク画像");
		if (isChangeMask_) {
			ImGui::Text("noise1");
		}
		else {
			ImGui::Text("noise0");
		}
#endif
		//溶かし具合
		if (Input::GetInstance().PushKey(DIK_D)) {
			threshold_->degree += 0.01f;
		}
		else if (Input::GetInstance().PushKey(DIK_A)) {
			threshold_->degree -= 0.01f;
		}
		threshold_->degree = std::clamp(threshold_->degree, 0.0f, 1.0f);

		//Edge調節
		if (Input::GetInstance().PushKey(DIK_RIGHT)) {
			threshold_->edgeSize += 0.001f;
		}
		else if (Input::GetInstance().PushKey(DIK_LEFT)) {
			threshold_->edgeSize -= 0.001f;
		}
		threshold_->edgeSize = std::clamp(threshold_->edgeSize, 0.0f, 0.1f);

		//マスク変更
		if (Input::GetInstance().TriggerKey(DIK_RETURN)) {
			isChangeMask_ = !isChangeMask_;
		}

		if (isChangeMask_) {
			textureFileName_ = "resource/Sprite/noise1.png";
			TextureManager::GetInstance().LoadTexture(textureFileName_);
		}
		else {
			textureFileName_ = "resource/Sprite/noise0.png";
			TextureManager::GetInstance().LoadTexture(textureFileName_);
		}
	}

	void Dissolve::SetBackGround(D3D12_GPU_DESCRIPTOR_HANDLE gpu, const std::string& textureFile) {

		TextureGPU_ = gpu;
		isFade_ = true;

		textureFileName_ = "resource/Sprite/" + textureFile;
		TextureManager::GetInstance().LoadTexture(textureFileName_);
	}

	void Dissolve::Degree(float value) {
		threshold_->degree = value;
	}

	void Dissolve::EdgeSize(float value) {
		threshold_->edgeSize = value;
	}
}