#include "SceneManager.h"

SceneManager::SceneManager() {}

SceneManager::~SceneManager() {}

std::unique_ptr<SceneManager> SceneManager::sInstance_ = nullptr;

SceneManager& SceneManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<SceneManager>();
	}
	return *sInstance_;
}

void SceneManager::ChangeScene(const std::string& sceneName) {
	//すでに値が入っている時
	if (nextScene_ != nullptr) {
		return;
	}
	assert(sceneFactory_);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
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
		scene_.swap(nextScene_);
		nextScene_.reset();
		nextScene_ = nullptr;
		//初期化処理
		scene_->Initialize();
	}
}

void SceneManager::Draw() {
	//描画処理
	scene_->Draw();
}

void SceneManager::Finalize() {
	scene_->Finalize();
	//シーンのリセット
	scene_->Finalize();
}

bool SceneManager::NextSceneChangeFlag() {
	//次のシーンの値が入っている時
	if (nextScene_) {
		return true;
	}
	return false;
}
