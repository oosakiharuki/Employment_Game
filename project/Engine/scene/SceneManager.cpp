#include "SceneManager.h"
#include "LoadingModels.h"

SceneManager::SceneManager() {
	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();
	
	//シーンの設定
	currentScene_ = sceneArr->GetSceneNo();
	prevScene_ = currentScene_;

	//シーンを作る
	BuildScene();
}

SceneManager::~SceneManager() {
	Finalize();
}

void SceneManager::SceneChange() {

	//前のシーンの解放
	Finalize();
	//次のシーンを作る
	BuildScene();
}

void SceneManager::BuildScene() {
	//シーンを作成
	sceneFactory.MakeScene(currentScene_);
	//代入
	sceneArr = sceneFactory.GetSceneGroup(currentScene_);
}

void SceneManager::Initialize() {
	//初期化処理
	sceneArr->Initialize();
}

void SceneManager::Update() {

	prevScene_ = currentScene_;
	currentScene_ = sceneArr->GetSceneNo();

	// シーンを変更(現在のシーンが前回のシーンと同じでない)
	if (prevScene_ != currentScene_) {
		SceneChange();
		sceneArr->Initialize();
	}

	//更新処理
	sceneArr->Update();
}

void SceneManager::Draw() {
	//描画処理
	sceneArr->Draw();
}

void SceneManager::Finalize() {
	sceneArr->Finalize();
	//シーンのリセット
	sceneArr.reset();
}
