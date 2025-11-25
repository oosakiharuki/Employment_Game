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

	const Vector2 kSpriteTranslateMove_ = { 300,20 };
	const Vector2 kSpriteTranslateEkey_ = { 600,20 };
};