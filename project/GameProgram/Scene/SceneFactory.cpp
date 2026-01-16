#include "SceneFactory.h"
#include "GameScene.h"//geme
#include "TitleScene.h"//title
#include "SelectScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"
#include "LoadingStageScene.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {

	BaseScene* newScene{};

	//タイトル
	if (sceneName == "Title") {
		newScene = new TitleScene();
	}
	//セレクトシーン
	else if (sceneName == "Select") {
		newScene = new SelectScene();
	}
	//ゲームシーン
	else if (sceneName == "Game") {
		newScene = new GameScene();
	}
	//ゲームオーバーシーン
	else if (sceneName == "GameOver") {
		newScene = new GameOverScene();
	}
	//クリアシーン
	else if (sceneName == "Clear") {
		newScene = new ClearScene();
	}
	//ローディングシーン(ステージの変更時)
	else if (sceneName == "NextStage") {
		newScene = new LoadingStageScene();
	}

	return newScene;//std::moveかも
}