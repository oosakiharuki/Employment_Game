#include "SceneManager.h"
#include "LoadingModels.h"

SceneManager::SceneManager() {
	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();


	sceneArr_[Title] = std::make_unique<TitleScene>();

	prevSceneNo_ = 0;
	currentSceneNo_ = Title;
}

SceneManager::~SceneManager() {
	Finalize();
}

void SceneManager::SceneChange(int prev, int current) {

	//前のシーンの解放
	Finalize();

	switch (current)
	{
	case Title:
		sceneArr_[current] = std::make_unique<TitleScene>();
		break;
	case Game:
		sceneArr_[current] = std::make_unique<GameScene>();
		break;	
	case Select:
		sceneArr_[current] = std::make_unique<SelectScene>();
		break;
	case Clear:
		sceneArr_[current] = std::make_unique<ClearScene>();
		break;
	case GameOver:
		sceneArr_[current] = std::make_unique<GameOverScene>();
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

void SceneManager::Finalize() {
	sceneArr_[currentSceneNo_].reset();
}
