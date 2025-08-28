#include "TitleScene.h"
#include "LoadingModels.h"

void TitleScene::Initialize() {
	sprite_Moji_Title = new Sprite();
	sprite_Moji_Title->Initialize("Moji_Title.png");
	sprite_Moji_Title->SetPosition({ 100,100 });

	sprite_Moji_Start = new Sprite();
	sprite_Moji_Start->Initialize("Moji_Title_Start.png");
	sprite_Moji_Start->SetPosition({ 800,500 });

	sprite_Moji_End = new Sprite();
	sprite_Moji_End->Initialize("Moji_Title_End.png");
	sprite_Moji_End->SetPosition({ 700,600 });

	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();

}

void TitleScene::Update() {
	sprite_Moji_Title->Update();
	sprite_Moji_Start->Update();
	sprite_Moji_End->Update();


	if (isfadeStart) {
		FadeScreen::GetInstance()->FedeIn();

		if (!FadeScreen::GetInstance()->GetIsFadeing()) {
			sceneNo = Select;
			isfadeStart = false;
		}
	}
	else {
		FadeScreen::GetInstance()->FedeOut();
	}

	//フェードが終わったら押せる
	if (!FadeScreen::GetInstance()->GetIsFadeing()) {
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			isfadeStart = true;
		}
	}

}

void TitleScene::Draw() {
	SpriteCommon::GetInstance()->Command();

	sprite_Moji_Title->Draw();
	sprite_Moji_Start->Draw();
	sprite_Moji_End->Draw();
}

void TitleScene::Finalize() {
	delete sprite_Moji_Title;
	delete sprite_Moji_Start;
	delete sprite_Moji_End;
}