#pragma once
#include "IScene.h"

/// <summary>
/// セレクト(ステージ選択)シーン(ISceneの派生クラス)
/// </summary>
class SelectScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:

	const uint32_t kPlayerLife_ = 2;  //残り残機 2 ～ 0 まで

	const Vector2 kSpriteTranslateMove_ = { 300,20 };
	const Vector2 kSpriteTranslateEkey_ = { 600,20 };
};