#include "PostEffectManager.h"


PostEffectManager* PostEffectManager::instance = nullptr;

uint32_t PostEffectManager::kSRVIndexTop = 1;

PostEffectManager* PostEffectManager::GetInstance() {
	if (instance == nullptr) {
		instance = new PostEffectManager;
	}
	return instance;
}

void PostEffectManager::Finalize() {
	effectArr_[currentNo_]->Finalize();
	delete effectArr_[currentNo_];

	delete instance;
	instance = nullptr;
}

void PostEffectManager::Change(int prev, int current) {

	//前のシーンの解放
	effectArr_[prev]->Finalize();
	delete effectArr_[prev];
	effectArr_[prev] = nullptr;

	//scene_ = current;
	switch (current)
	{
	case Mode_Normal_Image:
		effectArr_[current] = new Normal_Image();
		break;
	case Mode_BoxFillter:
		effectArr_[current] = new BoxFilter();
		break;
	case Mode_DepthBasedOutline:
		effectArr_[current] = new DepthBasedOutline();
		break;
	case Mode_Dissolve:
		effectArr_[current] = new Dissolve();
		break;
	case Mode_GaussianFillter:
		effectArr_[current] = new GaussianFilter();
		break;
	case Mode_Grayscale:
		effectArr_[current] = new Grayscale();
		break;
	case Mode_LuminanceBacedOutline:
		effectArr_[current] = new LuminanceBasedOutline();
		break;
	case Mode_RadialBlur:
		effectArr_[current] = new RadialBlur();
		break;
	case Mode_Random:
		effectArr_[current] = new Random();
		break;
	case Mode_Vignette:
		effectArr_[current] = new Vignette();
		break;
	default:
		break;
	}
	
}
void PostEffectManager::Initialize(DirectXCommon* dxCommon) {
	
	
	effectArr_[Mode_Normal_Image] = new Normal_Image();

	prevNo_ = 0;
	currentNo_ = Mode_Normal_Image;
	

	dxCommon_ = dxCommon;
	effectArr_[currentNo_]->Initialize(dxCommon_);	
}

void PostEffectManager::Update() {

	prevNo_ = currentNo_;
	currentNo_ = effectArr_[currentNo_]->GetEffectNo();

	if (prevNo_ != currentNo_) {
		Change(prevNo_, currentNo_);
		effectArr_[currentNo_]->Initialize(dxCommon_);
	}
#ifdef _DEBUG
	ImGui::Begin("PostEffect");
	ImGui::Text("スペースキーでポストエフェクトを変更できます");
#endif
	
	effectArr_[currentNo_]->EffectUpdate();//更新処理
	effectArr_[currentNo_]->ChangeNumber();//ポストエフェクト変更処理

#ifdef _DEBUG
	ImGui::End();
#endif	

}

void PostEffectManager::Command() {
	effectArr_[currentNo_]->Command();
}
