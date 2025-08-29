#include "ClearScene.h"

void ClearScene::Initialize() {
	sprite = new Sprite();
	sprite->Initialize("Moji_GameClear.png");
	sprite->SetPosition({ 100,100 });
}

void ClearScene::Update() {
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

void ClearScene::Draw() {
	SpriteCommon::GetInstance()->Command();

	sprite->Draw();
}

void ClearScene::Finalize() {
	delete sprite;

}