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
	if (Input::GetInstance().TriggerKey(DIK_ESCAPE)) {
		pauseScreen.SetPauseState(std::make_unique<PauseMove>());
	}
}
void PauseSelectGuide::Draw(PauseScreen& pauseScreen) {
	pauseScreen.DrawGuide();
}

void PauseSelectSceneChange::Update(PauseScreen& pauseScreen) {
	SceneManager::GetInstance().ChangeScene(pauseScreen.GetNextScene());
	pauseScreen.SetPauseState(std::make_unique<PauseMove>());
}
void PauseSelectSceneChange::Draw(PauseScreen& pauseScreen) {}
