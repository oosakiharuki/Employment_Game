#include "DepthBasedOutline.h"
#include <SrvManager.h>

using namespace Logger;
using namespace MyMath;

void DepthBasedOutline::Finalize() {}

void DepthBasedOutline::RootSignature() {

	//RootSignature
	PostEffectRootSignatureCommon();

	CreateDescriptorRange(descriptorRangeOutline_,1);//t1用

	CreateTABLE(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRangeOutline_);//[1] ps t1
	rootParameters_[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeOutline_);

	CreateCBV(D3D12_SHADER_VISIBILITY_PIXEL, 0);//[2] ps b1

	//SamplerPoint s1
	DefaultSampler(1);
	staticSamplers_[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT; 
	staticSamplers_[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//clamp = そのテクスチャが伸びる
	staticSamplers_[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	IntroduceRootParameters();
	IntroduceSamplers();
}

void DepthBasedOutline::CreateBlend() {
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void DepthBasedOutline::CreateRasterizer() {
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void DepthBasedOutline::CreatePixelShader() {
	pixelShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/DepthBasedOutline.PS.hlsl", L"ps_6_0");//ココのみ変化させる
	assert(pixelShaderBlob != nullptr);
}

void DepthBasedOutline::Command() {
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	DirectXCommon::GetInstance().GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	//通常の描画
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(0, srvHandleGPU_);
	//outlineの描画
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootDescriptorTable(1, srvHandleGPUDepth_);
	DirectXCommon::GetInstance().GetCommandList()->SetGraphicsRootConstantBufferView(2, materialResource_->GetGPUVirtualAddress());
	DirectXCommon::GetInstance().GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void DepthBasedOutline::EffectInit() {
	srvIndex_ = SrvManager::GetInstance().Allocate();
	srvHandleCPU_ = SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex_);

	SrvManager::GetInstance().CreateSRVForTexture2D(srvIndex_, DirectXCommon::GetInstance().GetRenderTexture(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 1);

	srvIndex_ = SrvManager::GetInstance().Allocate();
	srvHandleCPUDepth_ = SrvManager::GetInstance().GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPUDepth_ = SrvManager::GetInstance().GetGPUDescriptorHandle(srvIndex_);

	SrvManager::GetInstance().CreateSRVForTexture2D(srvIndex_, DirectXCommon::GetInstance().GetOutlineResource(), DXGI_FORMAT_R24_UNORM_X8_TYPELESS, 1);

	//Model用マテリアル
	//マテリアル用のリソース
	materialResource_ = DirectXCommon::GetInstance().CreateBufferResource(sizeof(DepthOutlineFunction));
	//書き込むためのアドレス
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&depthOutlineFunction_));
	//色の設定
	depthOutlineFunction_->projectionInverse = 150.0f;
}

void DepthBasedOutline::EffectUpdate() {

#ifdef USE_IMGUI
	ImGui::Text("DepthBasedOutline");
	//ImGui::SliderFloat("線", &depthOutlineFunction->projectionInverse, 0.0f, 100.0f);
#endif

}
