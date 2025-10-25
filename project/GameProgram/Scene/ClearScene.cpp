#include "ClearScene.h"

void ClearScene::Initialize() {
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("Moji_GameClear.png");
	sprite->SetPosition({ 100,100 });
}

void ClearScene::Update() {

	Input::GetInstance()->GetJoyStickState(0, state);
	Input::GetInstance()->GetJoystickStatePrevious(0, preState);

	sprite->Update();

	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
	}

	if (Input::GetInstance()->TriggerKey(DIK_SPACE) || 
		Input::GetInstance()->TriggerBotton(state,preState,XINPUT_GAMEPAD_A)) {
		NextSceneFadeInStart(Select);
	}

}

void ClearScene::Draw() {
	SpriteCommon::GetInstance()->Command();

	sprite->Draw();
}

void ClearScene::Finalize() {}