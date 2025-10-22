#pragma once 
#include "Framework.h"
#include "SceneManager.h"
#include "FadeScreen.h"

class MyGame : public Framework{
public:
	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

private:
	SceneManager* gameScene = nullptr;
	FadeScreen* fadeScreen = nullptr;

};