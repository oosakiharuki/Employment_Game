#pragma once
#include "BaseScene.h"
class LoadingStageScene : public BaseScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
	void SceneUpdate() override;
private:
	std::unique_ptr<Sprite> sprite_;
};

