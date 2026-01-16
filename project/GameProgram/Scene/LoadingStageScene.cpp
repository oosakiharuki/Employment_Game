#include "LoadingStageScene.h"
#include "SceneManager.h"

void LoadingStageScene::Initialize() {
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Loading_moji.png");
}

void LoadingStageScene::Update() {
	sprite_->Update();
}

void LoadingStageScene::Draw() {
	SpriteCommon::GetInstance().Command();
	sprite_->Draw();
}

void LoadingStageScene::Finalize() {}

void LoadingStageScene::SceneUpdate() {
	//すぐさまゲームシーンに
	SceneManager::GetInstance().ChangeScene("Game");
}
