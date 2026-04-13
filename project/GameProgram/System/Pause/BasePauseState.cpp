#include "BasePauseState.h"
#include "PauseScreen.h"
#include "SceneManager.h"

void PauseMove::Update(PauseScreen& pauseScreen) {
	pauseScreen.MoveSprite();
	pauseScreen.SelectMode();
}
void PauseMove::Draw(PauseScreen& pauseScreen) {
	pauseScreen.DrawSelectMode();
}

void PauseSelectGuide::Update(PauseScreen& pauseScreen) {
	pauseScreen.UpdateGuide();
	if (Input::GetInstance().TriggerKey(DIK_ESCAPE) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_START)) {
		pauseScreen.ChangePauseState(std::make_unique<PauseMove>());//ポーズ選択画面に戻る
	}
}
void PauseSelectGuide::Draw(PauseScreen& pauseScreen) {
	pauseScreen.DrawGuide();//操作ガイドの表示
}

void PauseSelectSceneChange::Update(PauseScreen& pauseScreen) {
	SceneManager::GetInstance().ChangeScene(pauseScreen.GetNextScene());
	pauseScreen.ChangePauseState(std::make_unique<PauseMove>());
}
void PauseSelectSceneChange::Draw(PauseScreen& pauseScreen) {}
