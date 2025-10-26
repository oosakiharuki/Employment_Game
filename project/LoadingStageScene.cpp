#include "LoadingStageScene.h"

void LoadingStageScene::Initialize() {
	sprite = std::make_unique<Sprite>();
	sprite->Initialize("Loading_moji.png");

	FadeScreen::GetInstance()->FadeStart(type_fadeOut);
}

void LoadingStageScene::Update() {

	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		ChangeScene();
	}
	sprite->Update();

	if (!FadeScreen::GetInstance()->GetIsFadeing()) {
		NextSceneFadeInStart("Game");
	}

}

void LoadingStageScene::Draw() {
	SpriteCommon::GetInstance()->Command();
	sprite->Draw();
}

void LoadingStageScene::Finalize() {}

