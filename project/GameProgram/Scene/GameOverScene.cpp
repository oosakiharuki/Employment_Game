#include "GameOverScene.h"

void GameOverScene::Initialize() {
}

void GameOverScene::Update() {

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		sceneNo = Game;
	}

}

void GameOverScene::Draw() {

}

void GameOverScene::Finalize() {

}