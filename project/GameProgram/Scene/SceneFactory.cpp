#include "SceneFactory.h"
#include "GameScene.h"//geme
#include "TitleScene.h"//title
#include "SelectScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {

	std::unique_ptr<BaseScene> newScene{};

	//タイトル
	if (sceneName == "Title") {
		newScene = std::make_unique<TitleScene>();
	}
	//セレクトシーン
	else if (sceneName == "Select") {
		newScene = std::make_unique<SelectScene>();
	}
	//ゲームシーン
	else if (sceneName == "Game") {
		newScene = std::make_unique<GameScene>();
	}
	//ゲームオーバーシーン
	else if (sceneName == "GameOver") {
		newScene = std::make_unique<GameOverScene>();
	}
	//クリアシーン
	else if (sceneName == "Clear") {
		newScene = std::make_unique<ClearScene>();
	}

	return std::move(newScene);//std::moveかも
}