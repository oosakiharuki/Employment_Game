#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "IScene.h"

class ClearScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	std::unique_ptr<Sprite> sprite;
};