#include "LoadingStageScene.h"

void LoadingStageScene::Initialize() {
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("Loading_moji.png");
}

void LoadingStageScene::Update() {
	sprite->Update();
}

void LoadingStageScene::Draw() {
	sprite->Draw();
}

void LoadingStageScene::Finalize() {}

