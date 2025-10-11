#pragma once
#include "Framework.h"
#include "IScene.h"

class GameOverScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	std::unique_ptr<Sprite> sprite;

};