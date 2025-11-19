#include "FadeScreen.h"
#include "MyMath.h"
#include "SpriteCommon.h"
using namespace MyMath;

FadeScreen* FadeScreen::sInstance= nullptr;

FadeScreen* FadeScreen::GetInstance() {
	if (sInstance== nullptr) {
		sInstance= new FadeScreen;
	}
	return sInstance;
}

void FadeScreen::Initialize() {
	//フェードのテクスチャ
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("fadeTexture.png");
	sprite_->SetSize({ 1280,720 });

	//解け具合
	dissolve_ = std::make_unique<Dissolve>();
	dissolve_->Initialize(DirectXCommon::GetInstance());
	
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
	dissolve_->EgdeSize(0.01f);
}

void FadeScreen::Draw() {
	dissolve_->Command();
}

void FadeScreen::Finalize() {
	delete sInstance;
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
	if (degress_ >= 1.0f) {
		degress_ = 1.0f;
		isFading_ = false;
		return;
	}

	degress_ += kDeltaTime_;
}

