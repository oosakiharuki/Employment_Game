#include "GameOverScene.h"

void GameOverScene::Initialize() {
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("Moji_GameOver.png");
	sprite->SetPosition({ 100,100 });
}

void GameOverScene::Update() {

	Input::GetInstance()->GetJoyStickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);
	
	sprite->Update();

	if (!FadeScreen::GetInstance()->GetIsFadeing()) {
		ChangeScene();
	}

	if (Input::GetInstance()->TriggerKey(DIK_SPACE) ||
		Input::GetInstance()->TriggerBotton(state, preState, XINPUT_GAMEPAD_A)) {
		NextSceneFadeInStart("Select");
	}

}

void GameOverScene::Draw() {
	SpriteCommon::GetInstance()->Command();

	sprite->Draw();

}

void GameOverScene::Finalize() {}