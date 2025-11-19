#include "SceneFactory.h"
#include "GameScene.h"//geme
#include "TitleScene.h"//title
#include "SelectScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"
#include "LoadingStageScene.h"

void SceneFactory::MakeScene(const std::string preScene) {
	
	// シーンの名前が同じであれば
	//タイトル
	if (preScene == "Title") {
		sceneG_[preScene] = std::make_unique<TitleScene>();
	}
	//セレクトシーン
	else if(preScene == "Select") {
		sceneG_[preScene] = std::make_unique<SelectScene>();
	}
	//ゲームシーン
	else if (preScene == "Game") {
		sceneG_[preScene] = std::make_unique<GameScene>();
	}
	//ゲームオーバーシーン
	else if (preScene == "GameOver") {
		sceneG_[preScene] = std::make_unique<GameOverScene>();
	}
	//クリアシーン
	else if(preScene == "Clear") {
		sceneG_[preScene] = std::make_unique<ClearScene>();
	}
	//ローディングシーン(ステージの変更時)
	else if(preScene == "NextStage") {
		sceneG_[preScene] = std::make_unique<LoadingStageScene>();
	}
}

std::unique_ptr<IScene> SceneFactory::GetSceneGroup(std::string name) {
	return std::move(sceneG_[name]);
}