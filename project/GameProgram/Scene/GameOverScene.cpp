#include "GameOverScene.h"

void GameOverScene::Initialize() {
	sprite = new Sprite();
	sprite->Initialize("Moji_GameOver.png");
	sprite->SetPosition({ 100,100 });
}

void GameOverScene::Update() {

	Input::GetInstance()->GetJoyStickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);
	
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

	if (Input::GetInstance()->TriggerKey(DIK_SPACE) ||
		Input::GetInstance()->TriggerBotton(state, preState, XINPUT_GAMEPAD_A)) {
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