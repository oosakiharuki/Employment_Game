#include "BaseScene.h"
#include "SceneManager.h"

using namespace MyMath;
using namespace UseEveryOne;

std::string BaseScene::sceneNo_ = "Game";
std::string BaseScene::nextSceneNo_ = sceneNo_;

BaseScene::~BaseScene(){}

std::string BaseScene::GetSceneNo() { return sceneNo_; }

void BaseScene::NextSceneFadeInStart(const std::string& name) {
	//フェードイン開始
	FadeScreen::GetInstance()->FadeStart(type_fadeIn);
	//次のステージ名
	nextSceneNo_ = name;
}

bool BaseScene::NextSceneFlag() {
	//現在のシーンと次のシーンが違う場合(例: Select , Game true / Select , Select false)
	if (sceneNo_ != nextSceneNo_) {
		return true;// 次のシーンに進む
	}
	return false;// シーン移動しない
}

void BaseScene::ChangeScene() {
	//ステージの変更
	sceneNo_ = nextSceneNo_;
}

std::unique_ptr<BaseScene> BaseScene::SetCurrentScene() { 
	std::unique_ptr<BaseScene> currentScene_ = nullptr;
	SceneFactory sceneFactory;

	sceneFactory.MakeScene(sceneNo_);
	currentScene_ = sceneFactory.GetSceneGroup(sceneNo_);

	return currentScene_;
};