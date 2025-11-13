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
	//フェードのテクスチャ
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("fadeTexture.png");
	sprite->SetSize({ 1280,720 });

	//解け具合
	dissolve = std::make_unique<Dissolve>();
	dissolve->Initialize(DirectXCommon::GetInstance());
	
	//マウスのテクスチャ
	dissolveTexture = "fade01.png";
}

void FadeScreen::Update() {

	//フェード中である
	if (isFading) {
		switch (FadeType_)
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

	sprite->Update();
	//フェードするテクスチャを導入
	dissolve->SetBackGround(sprite->GetResource(),dissolveTexture);

	//溶かす度合
	dissolve->Degress(degress);
	dissolve->EgdeSize(0.01f);
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

	degress -= kDeltaTime;
}

void FadeScreen::FadeOut() {
	if (degress >= 1.0f) {
		degress = 1.0f;
		isFading = false;
		return;
	}

	degress += kDeltaTime;
}

