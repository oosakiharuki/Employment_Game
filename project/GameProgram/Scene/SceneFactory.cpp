#include "SceneFactory.h"
#include "GameScene.h"//geme
#include "TitleScene.h"//title
#include "SelectScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"

void SceneFactory::AddScene() {
	
	
	sceneG["Title"].iscene = std::make_unique<TitleScene>();
	sceneG["Select"].iscene = std::make_unique<SelectScene>();
	sceneG["Game"].iscene = std::make_unique<GameScene>();
	sceneG["GameOver"].iscene = std::make_unique<GameOverScene>();
	sceneG["Clear"].iscene = std::make_unique<ClearScene>();

}

SceneGroup SceneFactory::GetSceneGroup(std::string name) {

	SceneGroup& scene = sceneG[name];

	return scene;
}