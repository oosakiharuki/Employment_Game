#pragma once
#include "Framework.h"
#include "IScene.h"

/// <summary>
/// ゲームオーバーシーン(ISceneの派生クラス)
/// </summary>
class GameOverScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	std::unique_ptr<Sprite> sprite_;//ゲームオーバーの文字
	std::unique_ptr<Sprite> spriteSpace_;//Spaceでもどるの文字

	std::unique_ptr<Object_glTF> playerGltf_;
	WorldTransform wt_;
	
	std::unique_ptr<Object_glTF> stageGltf_;

	const Vector3 kCameraTranslate_ = { 0.0f,5.0f,-15.0f };
	const Vector3 kCameraRotate_ = { 15.0f,0.0f,0.0f };

	const Vector2 kSpritePositionGameOver_ = { 100.0f,100.0f };
	const Vector2 kSpritePositionBotton_ = { 800.0f,10.0f };
	const Vector2 kSpriteSizeBotton_ = { 256.0f, 64.0f };


	const float kRotate_ = 0.5f;
};