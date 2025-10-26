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

	dissolve = std::make_unique<Dissolve>();
	dissolve->Initialize(DirectXCommon::GetInstance());
	//フェードするテクスチャを導入
	dissolve->SetBackGround(sprite->GetResource());
}

void FadeScreen::Update() {

	if (isFading) {
		switch (FadeType_)
		{
		case type_fadeIn:
			FadeIn();
			break;
		case type_fadeOut:
			FadeOut();
			break;
		default:
			break;
		}
	}


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

void FadeScreen::FadeIn() {
	if (degress <= 0.0f) {
		degress = 0.0f;
		isFading = false;
		return;
	}

	degress -= deltaTime;
}

void FadeScreen::FadeOut() {
	if (degress >= 1.0f) {
		degress = 1.0f;
		isFading = false;
		return;
	}

	degress += deltaTime;
}

