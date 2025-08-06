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
	Mode_LuminanceBacedOutline,
	Mode_DepthBasedOutline,
	Mode_RadialBlur,
	Mode_Dissolve,
	Mode_Random,
	Max,//最大
};

class IPostEffects {
protected:
	static int effectNo;
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
};