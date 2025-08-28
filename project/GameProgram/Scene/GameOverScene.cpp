#include "GameOverScene.h"

void GameOverScene::Initialize() {
	sprite = new Sprite();
	sprite->Initialize("Moji_GameOver.png");
	sprite->SetPosition({ 100,100 });
}

void GameOverScene::Update() {
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

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isfadeStart = true;
	}

}

void GameOverScene::Draw() {
	SpriteCommon::GetInstance()->Command();

	sprite->Draw();

}

void GameOverScene::Finalize() {
	delete sprite;
}