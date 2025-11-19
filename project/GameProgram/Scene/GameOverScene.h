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
};