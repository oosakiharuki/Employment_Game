#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "IScene.h"

class TitleScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	Sprite* sprite_Moji_Title;
	Sprite* sprite_Moji_Start;
	Sprite* sprite_Moji_End;
};