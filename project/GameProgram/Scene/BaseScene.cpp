#include "BaseScene.h"
#include "SceneManager.h"

using namespace MyMath;
using namespace UseEveryOne;

std::string BaseScene::sceneNo_ = "Game";
std::string BaseScene::nextSceneNo_ = sceneNo_;

BaseScene::~BaseScene(){}

std::string BaseScene::GetSceneNo() { return sceneNo_; }

bool BaseScene::NextSceneFlag() {
	//現在のシーンと次のシーンが違う場合(例: Select , Game true / Select , Select false)
	if (sceneNo_ != nextSceneNo_) {
		return true;// 次のシーンに進む
	}
	return false;// シーン移動しない
}

void BaseScene::ChangeSceneNo() {
	//フェードイン開始
	if (NextSceneFlag() && !isFade_) {
		FadeScreen::GetInstance()->FadeStart(type_fadeIn);
		isFade_ = true;
	}

	//フェード中でないか && 次のシーンに変更フラグが立ったか
	if (!FadeScreen::GetInstance()->GetIsFadeing() && NextSceneFlag()) {
		//シーンの変更
		sceneNo_ = nextSceneNo_;
		isFade_ = false;
	}

	//"End"の場合ゲーム終了
	if (sceneNo_ == "End") {
		isGameEnd_ = true;
	}
}

std::unique_ptr<BaseScene> BaseScene::SetCurrentScene() { 
	std::unique_ptr<BaseScene> currentScene_ = nullptr;
	SceneFactory sceneFactory;

	sceneFactory.MakeScene(sceneNo_);
	currentScene_ = sceneFactory.GetSceneGroup(sceneNo_);

	return currentScene_;
};