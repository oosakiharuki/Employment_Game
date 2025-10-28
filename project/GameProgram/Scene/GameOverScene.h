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
	std::unique_ptr<Sprite> sprite;//ゲームオーバーの文字
	std::unique_ptr<Sprite> sprite_space;//Spaceでもどるの文字

	std::unique_ptr<Object_glTF> player_gltf;
	WorldTransform wt;
	
	std::unique_ptr<Object_glTF> stage_gltf;
};