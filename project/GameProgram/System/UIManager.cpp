#include "UIManager.h"

std::unique_ptr<UIManager> UIManager::sInstance_ = nullptr;

UIManager& UIManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<UIManager>();
	}
	return *sInstance_;
}

void UIManager::Finalize() {
	sInstance_.reset();
}

void UIManager::FrameSprite(EngineLayer::Sprite* sprite) {
	frameSprites_.push_back(sprite);
}
void UIManager::FixedSprite(EngineLayer::Sprite* sprite) {
	fixedSprites_.push_back(sprite);
}
void UIManager::Update() {
	//フレーム更新と一度読み取りを合わせる
	for (auto& sprite : fixedSprites_) {
		frameSprites_.push_back(sprite);
	}
	//更新処理
	for (auto& sprite : frameSprites_) {
		sprite->Update();
	}
}

void UIManager::Draw() {
	//描画処理
	for (auto& sprite : frameSprites_) {
		sprite->Draw();
	}
	//役目を終えたためリセット
	frameSprites_.clear();
}
