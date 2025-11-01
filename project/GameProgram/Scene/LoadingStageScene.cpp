#include "LoadingStageScene.h"

void LoadingStageScene::Initialize() {
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("Loading_moji.png");
}

void LoadingStageScene::Update() {

	if (NextSceneFlag()) {
		ChangeScene();
	}
	sprite->Update();

	//すぐさまゲームシーンに
	NextSceneFadeInStart("Game");
}

void LoadingStageScene::Draw() {
	SpriteCommon::GetInstance()->Command();
	sprite->Draw();
}

void LoadingStageScene::Finalize() {}

