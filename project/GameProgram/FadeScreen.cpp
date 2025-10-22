#include "FadeScreen.h"
#include "MyMath.h"
#include "SpriteCommon.h"
using namespace MyMath;

FadeScreen* FadeScreen::instance = nullptr;

FadeScreen* FadeScreen::GetInstance() {
	if (instance == nullptr) {
		instance = new FadeScreen;
	}
	return instance;
}

void FadeScreen::Initialize() {
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("fadeTexture.png");
	sprite->SetSize({ 1280,720 });

	postEffect_ = std::make_unique<Dissolve>();
	postEffect_->Initialize(DirectXCommon::GetInstance());

	dissolve = dynamic_cast<Dissolve*>(postEffect_.get());
	//フェードするテクスチャを導入
	dissolve->SetBackGround(sprite->GetResource());
}

void FadeScreen::Update() {
	sprite->Update();
	dissolve->Degress(degress);
}

void FadeScreen::Draw() {
	dissolve->Command();
}

void FadeScreen::Finalize() {
	delete instance;
	instance = nullptr;
}

void FadeScreen::FedeIn() {
	if (degress <= 0.0f) {
		degress = 0.0f;
		isFading = false;
		return;
	}

	degress -= deltaTime;
	isFading = true;
}

void FadeScreen::FedeOut() {
	if (degress >= 1.0f) {
		degress = 1.0f;
		isFading = false;
		return;
	}

	degress += deltaTime;
	isFading = true;
}

