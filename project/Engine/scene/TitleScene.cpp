#include "TitleScene.h"
#include "LoadingModels.h"

void TitleScene::Initialize() {
	sprite = new Sprite();
	sprite->Initialize("monsterBall.png");

	sprite->SetPosition({ 64,64 });
	sprite->SetSize({ 128,128 });

	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();

}

void TitleScene::Update() {
	sprite->Update();


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

	sprite->Draw();
}

void TitleScene::Finalize() {
	delete sprite;
}