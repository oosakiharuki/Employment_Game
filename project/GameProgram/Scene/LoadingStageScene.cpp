#include "LoadingStageScene.h"

void LoadingStageScene::Initialize() {
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Loading_moji.png");
}

void LoadingStageScene::Update() {

	if (NextSceneFlag()) {
		ChangeScene();
	}
	sprite_->Update();

	//すぐさまゲームシーンに
	NextSceneFadeInStart("Game");
}

void LoadingStageScene::Draw() {
	SpriteCommon::GetInstance()->Command();
	sprite_->Draw();
}

void LoadingStageScene::Finalize() {}

