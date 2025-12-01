#include "FadeScreen.h"
#include "MyMath.h"
#include "SpriteCommon.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

std::shared_ptr<FadeScreen> FadeScreen::sInstance= nullptr;

std::shared_ptr<FadeScreen> FadeScreen::GetInstance() {
	if (sInstance== nullptr) {
		sInstance= std::make_unique<FadeScreen>();
	}
	return sInstance;
}

void FadeScreen::Initialize() {
	//フェードのテクスチャ
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("fadeTexture.png");
	sprite_->SetSize({ WinApp::kClientWidth_,WinApp::kClientHeight_ });

	//解け具合
	dissolve_ = std::make_unique<Dissolve>();
	dissolve_->Initialize(DirectXCommon::GetInstance().get());
	
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
	dissolve_->Degress(degress_);
	dissolve_->EdgeSize(kEdgeSize);
}

void FadeScreen::Draw() {
	dissolve_->Command();
}

void FadeScreen::Finalize() {
	sInstance.reset();
	sInstance= nullptr;
}

void FadeScreen::FadeIn() {
	if (degress_ <= 0.0f) {
		degress_ = 0.0f;
		isFading_ = false;
		return;
	}

	degress_ -= kDeltaTime_;
}

void FadeScreen::FadeOut() {
	if (degress_ >= kDegressMax) {
		degress_ = kDegressMax;
		isFading_ = false;
		return;
	}

	degress_ += kDeltaTime_;
}

