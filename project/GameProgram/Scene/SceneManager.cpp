#include "SceneManager.h"
#include "LoadingModels.h"

SceneManager::SceneManager() {
	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();


	sceneArr_[Select] = new SelectScene();

	prevSceneNo_ = 0;
	currentSceneNo_ = Select;
}

SceneManager::~SceneManager() {
	sceneArr_[currentSceneNo_]->Finalize();
	delete sceneArr_[currentSceneNo_];
}

void SceneManager::SceneChange(int prev, int current) {

	//前のシーンの解放
	sceneArr_[prev]->Finalize();
	delete sceneArr_[prev];
	sceneArr_[prev] = nullptr;

	//scene_ = current;
	switch (current)
	{
	case Title:
		sceneArr_[current] = new TitleScene();
		break;
	case Game:
		sceneArr_[current] = new GameScene();
		break;	
	case Select:
		sceneArr_[current] = new SelectScene();
		break;
	case Clear:
		sceneArr_[current] = new ClearScene();
		break;
	case GameOver:
		sceneArr_[current] = new GameOverScene();
		break;
	}
}
void SceneManager::Initialize() {
	sceneArr_[currentSceneNo_]->Initialize();
}

void SceneManager::Update() {

	prevSceneNo_ = currentSceneNo_;
	currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();

	if (prevSceneNo_ != currentSceneNo_) {
		SceneChange(prevSceneNo_,currentSceneNo_);
		sceneArr_[currentSceneNo_]->Initialize();
	}

	sceneArr_[currentSceneNo_]->Update();
}

void SceneManager::Draw() {
	sceneArr_[currentSceneNo_]->Draw();
}