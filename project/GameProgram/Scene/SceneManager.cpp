#include "SceneManager.h"
#include "LoadingModels.h"

SceneManager::SceneManager() {
	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();

	sceneFactory.AddScene();
	sceneArr = sceneFactory.GetSceneGroup("Title");
}

SceneManager::~SceneManager() {
	Finalize();
}

void SceneManager::SceneChange() {

	//前のシーンの解放
	Finalize();
	sceneArr = sceneFactory.GetSceneGroup(currentSceneNo_);
}
void SceneManager::Initialize() {
	sceneArr.iscene->Initialize();
}

void SceneManager::Update() {

	prevSceneNo_ = currentSceneNo_;
	currentSceneNo_ = sceneArr.iscene->GetSceneNo();

	if (prevSceneNo_ != currentSceneNo_) {
		SceneChange();
		sceneArr.iscene->Initialize();
	}

	sceneArr.iscene->Update();
}

void SceneManager::Draw() {
	sceneArr.iscene->Draw();
}

void SceneManager::Finalize() {
	sceneArr.iscene.reset();
}
