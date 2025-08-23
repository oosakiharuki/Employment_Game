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
	sprite = new Sprite();
	sprite->Initialize("white.png");
	sprite->SetSize({ 1280,720 });
}

void FadeScreen::Update() {
	sprite->Update();
	sprite->SetColor(color);
}

void FadeScreen::Draw() {
	SpriteCommon::GetInstance()->Command();

	sprite->Draw();
}

void FadeScreen::Finalize() {
	delete sprite;
	delete instance;
	instance = nullptr;
}

void FadeScreen::FedeIn() {
	if (color.s >= 1.0f) {
		color.s = 1.0f;
		isFading = false;
		return;
	}

	color.s += t;

	isFading = true;

}

void FadeScreen::FedeOut() {
	if (color.s <= 0.0f) {
		color.s = 0.0f;
		isFading = false;
		return;
	}

	color.s -= t;

	isFading = true;

}

