#include "SceneManager.h"

SceneManager::SceneManager() {}

SceneManager::~SceneManager() {
	sInstance_.reset();
}

std::unique_ptr<SceneManager> SceneManager::sInstance_ = nullptr;

SceneManager& SceneManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<SceneManager>();
	}
	return *sInstance_;
}

void SceneManager::ChangeScene(std::unique_ptr<BaseScene> nextScene) {
	//すでに値が入っている時
	if (nextScene_ != nullptr) {
		return;
	}
	nextScene_ = std::move(nextScene);
}

void SceneManager::Update() {

	//更新処理
	scene_->Update();

	//現在シーン更新処理
	scene_->SceneUpdate();
}

void SceneManager::SceneUpdate() {
	// シーンを変更(現在のシーンが前回のシーンと同じでない)
	if (NextSceneChangeFlag()) {
		if (scene_) {
			scene_->Finalize();
			scene_.reset();
		}
		scene_ = std::move(nextScene_);
		//初期化処理
		scene_->Initialize();
	}
}

void SceneManager::Draw() {
	//描画処理
	scene_->Draw();
}

void SceneManager::Finalize() {
	//シーンのリセット
	scene_->Finalize();
	//シ－ンの開放
	scene_.reset();
	sInstance_.reset();
}

bool SceneManager::NextSceneChangeFlag() {
	//次のシーンの値が入っている時
	if (nextScene_) {
		return true;
	}
	return false;
}
