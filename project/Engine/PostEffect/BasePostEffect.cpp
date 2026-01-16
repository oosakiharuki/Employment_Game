#include "BasePostEffect.h"

int BasePostEffect::sEffectNo_ = Mode_DepthBasedOutline;

BasePostEffect::~BasePostEffect() {}

//素早く変更可能
void BasePostEffect::ChangeNumber() {

#ifdef _DEBUG
	if (Input::GetInstance().TriggerKey(DIK_F1)) {
		sEffectNo_++;
	}
	//最大→0に
	if (sEffectNo_ == Max) {
		sEffectNo_ = 0;
	}
#endif // _DEBUG
}

void BasePostEffect::Initialize() {
	GraphicsPipeline();
	EffectInit();
}

void BasePostEffect::CreateInputLayout() {
	InputElementDescCommon();//POSITIONとTEXCORD
	IntroduceInputElementDesc();//InputLayoutDescに導入する
}

void BasePostEffect::CreateBlend() {
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void BasePostEffect::CreateRasterizer() {
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;//表裏表示
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
}

void BasePostEffect::CreateVertexShader() {
	vertexShaderBlob = DirectXCommon::GetInstance().CompileShader(L"resource/shaders/Fullscreen.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
}

void BasePostEffect::CreateDepthStencil() {
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void BasePostEffect::PostEffectRootSignatureCommon() {

	//RootSignature
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CreateDescriptorRange(descriptorRange_, 0);

	//RootParameter作成__
	CreateTABLE(D3D12_SHADER_VISIBILITY_PIXEL, descriptorRange_);//[0] ps t0
	rootParameters_[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);

	//2でまとめる
	DefaultSampler(0);
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//clamp = そのテクスチャが伸びる
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

}

