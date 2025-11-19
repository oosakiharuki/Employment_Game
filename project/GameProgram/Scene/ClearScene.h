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

	bool setFrequencyTime_ = false;
};