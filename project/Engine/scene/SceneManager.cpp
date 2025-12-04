#include "SceneManager.h"
#include "LoadingModels.h"

SceneManager::SceneManager() {
	//objectをローディング
	LoadingModels::GetInstance()->LoadObjects();
	LoadingModels::GetInstance()->Finalize();
	
	//シーンの設定
	currentScene_ = sceneArr_->GetSceneNo();
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
	settingScene_ = sceneArr_->SetCurrentScene();
	//代入
	sceneArr_ = std::move(settingScene_);
}

void SceneManager::Initialize() {
	//初期化処理
	sceneArr_->Initialize();
}

void SceneManager::Update() {

	prevScene_ = currentScene_;
	currentScene_ = sceneArr_->GetSceneNo();

	// シーンを変更(現在のシーンが前回のシーンと同じでない)
	if (prevScene_ != currentScene_) {
		SceneChange();
		sceneArr_->Initialize();
	}

	//更新処理
	sceneArr_->Update();
}

void SceneManager::Draw() {
	//描画処理
	sceneArr_->Draw();
}

void SceneManager::Finalize() {
	sceneArr_->Finalize();
	//シーンのリセット
	sceneArr_.reset();
}
