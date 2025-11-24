#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "IScene.h"

/// <summary>
/// クリアシーン(ISceneの派生クラス)
/// </summary>
class ClearScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	std::unique_ptr<Sprite> sprite_;//ゲームクリアの文字
	std::unique_ptr<Sprite> spriteSpace_;//Spaceでもどるの文字

	std::unique_ptr<Object_glTF> playerGltf_;
	std::unique_ptr<Object_glTF> stageGltf_;
	WorldTransform wt_;

	std::vector<std::unique_ptr<Particle>> particle_fanfares_;

	const Vector3 kCameraTranslate_ = { 0.0f,2.0f,-18.0f };

	const Vector2 kSpritePosition_ = { 800,10 };
	const Vector2 kSpriteSize_ = { 256,64 };

	const float kPlayerFrontRange_ = 180.0f;

	const float kFanfareX_ = 3.0f;
	const Vector3 kFanfareInitTranslate_ = { -kFanfareX_,2,0 };
	Vector3 fanfareTranslate_ = kFanfareInitTranslate_;

};