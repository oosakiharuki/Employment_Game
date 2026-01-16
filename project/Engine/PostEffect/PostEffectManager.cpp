#include "PostEffectManager.h"

std::unique_ptr<PostEffectManager> PostEffectManager::sInstance_ = nullptr;

PostEffectManager& PostEffectManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<PostEffectManager>();
	}
	return *sInstance_;
}

void PostEffectManager::Finalize() {
	effectArr_[currentNo_]->Finalize();
	effectArr_[currentNo_].reset();
	sInstance_.reset();
}

void PostEffectManager::Change(int prev, int current) {

	//前のシーンの解放
	effectArr_[prev]->Finalize();
	effectArr_[prev].reset();
	effectArr_[prev] = nullptr;

	//scene_ = current;
	switch (current)
	{
	case Mode_Normal_Image:
		effectArr_[current] = std::make_unique<Normal_Image>();
		break;
	case Mode_BoxFillter:
		effectArr_[current] = std::make_unique<BoxFilter>();
		break;
	case Mode_DepthBasedOutline:
		effectArr_[current] = std::make_unique<DepthBasedOutline>();
		break;
	case Mode_Dissolve:
		effectArr_[current] = std::make_unique<Dissolve>();
		break;
	case Mode_GaussianFillter:
		effectArr_[current] = std::make_unique<GaussianFilter>();
		break;
	case Mode_Grayscale:
		effectArr_[current] = std::make_unique<Grayscale>();
		break;
	case Mode_LuminanceBasedOutline:
		effectArr_[current] = std::make_unique<LuminanceBasedOutline>();
		break;
	case Mode_RadialBlur:
		effectArr_[current] = std::make_unique<RadialBlur>();
		break;
	case Mode_Random:
		effectArr_[current] = std::make_unique<Random>();
		break;
	case Mode_Vignette:
		effectArr_[current] = std::make_unique<Vignette>();
		break;
	default:
		break;
	}
	
}
void PostEffectManager::Initialize() {
	//ポストエフェクト作成
	effectArr_[Mode_DepthBasedOutline] = std::make_unique<DepthBasedOutline>();

	prevNo_ = 0;
	currentNo_ = Mode_DepthBasedOutline;
	
	//
	effectArr_[currentNo_]->Initialize();	
}

void PostEffectManager::Update() {

	prevNo_ = currentNo_;
	currentNo_ = effectArr_[currentNo_]->GetEffectNo();

	if (prevNo_ != currentNo_) {
		Change(prevNo_, currentNo_);
		effectArr_[currentNo_]->Initialize();
	}
#ifdef USE_IMGUI
	ImGui::Begin("PostEffect");
	ImGui::Text("スペースキーでポストエフェクトを変更できます");
#endif
	
	effectArr_[currentNo_]->EffectUpdate();//更新処理
	effectArr_[currentNo_]->ChangeNumber();//ポストエフェクト変更処理

#ifdef USE_IMGUI
	ImGui::End();
#endif	

}

void PostEffectManager::Command() {
	effectArr_[currentNo_]->Command();
}
