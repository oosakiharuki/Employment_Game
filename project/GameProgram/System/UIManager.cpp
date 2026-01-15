#include "UIManager.h"

std::unique_ptr<UIManager> UIManager::sInstance_ = nullptr;

UIManager& UIManager::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<UIManager>();
	}
	return *sInstance_;
}

void UIManager::Finalize() {
	guides_.clear();
	sprites_.clear();
	
	sInstance_.reset();
}

void UIManager::CreateSprite(const SpriteData& spriteData) {
	std::unique_ptr<Sprite>& iterator = sprites_[spriteData.name];

	InitSprite(iterator, spriteData);
}

void UIManager::CreateGuide(const Guide& guide) {
	std::unique_ptr<Sprite>& iterator = spriteGuides_[guide.spriteData.name];
	
	InitSprite(iterator,guide.spriteData);

	//Guide構造体をvectorにまとめる
	guides_.push_back(guide);
}

void UIManager::InitSprite(std::unique_ptr<Sprite>& sprite, const SpriteData& spriteData) {
	sprite = std::make_unique<Sprite>();
	sprite->Initialize(spriteData.texturePath + ".png");//初期化
	sprite->SetSize(spriteData.size);          //サイズ設定
	sprite->SetPosition(spriteData.position); //座標設定
}

void UIManager::Update() {
	for (auto& sprite : sprites_) {
		sprite.second->Update();
	}

	for (auto& guide : spriteGuides_) {
		guide.second->Update();
	}
}

void UIManager::Draw() {
	for (auto& sprite : sprites_) {
		sprite.second->Draw();
	}
}

void UIManager::GuideDraw() {
	//チュートリアルの出る順番
	for (auto& guide : guides_) {
		//プレイヤーの座標Xがガイドの設定した左端と右端の間にいるか
		if (playerTranslate_.x >= guide.lookPointX_left &&
			playerTranslate_.x < guide.lookPointX_right) {
			spriteGuides_[guide.spriteData.name]->Draw();
		}
	}
}

void UIManager::SetSpriteTexture(const std::string name, const std::string& texturePath) {
	std::unique_ptr<Sprite>& iterator = sprites_[name];

	if (iterator->GetTextureFile() != texturePath + ".png") {
		iterator->SetTextureFile(texturePath + ".png");
	}
}

std::string UIManager::GetSpriteTexture(const std::string name) {
	std::unique_ptr<Sprite>& iterator = sprites_[name];

	return iterator->GetTextureFile();
}
