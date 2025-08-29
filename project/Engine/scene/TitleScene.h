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
	Object3d* Moji_Title;

	Sprite* sprite_Moji_Start;
	Sprite* sprite_Moji_End;
	Sprite* sprite_Select_Allow;

	float move = 0.0f;
	Vector3 center = { 0.0f,10.0f,0.0f };

	float timer = 0.0f;
	const float TimeMax = 1.0f;
};