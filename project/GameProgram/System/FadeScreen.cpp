#include "FadeScreen.h"
#include "MyMath.h"
#include "SpriteCommon.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

std::unique_ptr<FadeScreen> FadeScreen::sInstance_= nullptr;

FadeScreen& FadeScreen::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<FadeScreen>();
	}
	return *sInstance_;
}

void FadeScreen::Initialize() {
	//フェードのテクスチャ
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("fadeTexture.png");
	sprite_->SetSize({ WinApp::kClientWidth_,WinApp::kClientHeight_ });

	//解け具合
	dissolve_ = std::make_unique<Dissolve>();
	dissolve_->Initialize();
	
	//マウスのテクスチャ
	dissolveTexture_ = "fade01.png";
}

void FadeScreen::Update() {

	//フェード中である
	if (isFading_) {
		switch (fadeType_)
		{
		case type_fadeIn:
			FadeIn();//フェードテクスチャ -> ゲーム画面
			break;
		case type_fadeOut:
			FadeOut();//ゲーム画面 -> フェードテクスチャ
			break;
		default:
			break;
		}
	}

	sprite_->Update();
	//フェードするテクスチャを導入
	dissolve_->SetBackGround(sprite_->GetResource(),dissolveTexture_);

	//溶かす度合
	dissolve_->Degree(degree_);
	dissolve_->EdgeSize(kEdgeSize);
}

void FadeScreen::Draw() {
	dissolve_->Command();
}

void FadeScreen::Finalize() {
	sInstance_.reset();
	sInstance_ = nullptr;
}

void FadeScreen::FadeIn() {
	if (degree_ <= 0.0f) {
		degree_ = 0.0f;
		isFading_ = false;
		return;
	}

	degree_ -= kDeltaTime_;
}

void FadeScreen::FadeOut() {
	if (degree_ >= kDegreeMax) {
		degree_ = kDegreeMax;
		isFading_ = false;
		return;
	}

	degree_ += kDeltaTime_;
}

