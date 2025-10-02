#pragma once
#include <cstdint>
#include "DirectXCommon.h"
#include "Input.h"
#include "ImGuiManager.h"

enum EFFECT {
	Mode_Normal_Image,//None 何もなし
	Mode_Grayscale,
	Mode_Vignette,
	Mode_BoxFillter,
	Mode_GaussianFillter,
	Mode_LuminanceBasedOutline,
	Mode_DepthBasedOutline,
	Mode_RadialBlur,
	Mode_Dissolve,
	Mode_Random,
	Max,//最大
};

class IPostEffects {
protected:
	//postEffect特融処理

	static int effectNo;

	DirectXCommon* dxCommon_;

	uint32_t srvIndex;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

	///DepthOutlineで使う
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU2;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU2;

	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;

	///RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//他はPixselShader等で配列が変わったりするため固有にする

public:
	virtual void Finalize() = 0;
	virtual void Initialize(DirectXCommon* dxCommon) = 0;

	virtual void Command() = 0;
	
	int GetEffectNo() { return effectNo; }

	//PSO
	virtual void RootSignature() = 0;
	virtual void GraphicsPipeline() = 0;

	virtual void EffectUpdate() = 0;

	virtual ~IPostEffects();

	void ChangeNumber();

	D3D12_GPU_DESCRIPTOR_HANDLE GetEffect() { return srvHandleGPU; }
	//DirectXCommon* GetDirectXCommon()const { return dxCommon_; }
};