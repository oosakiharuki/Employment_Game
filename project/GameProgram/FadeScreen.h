#pragma once
#include "Sprite.h"

class FadeScreen {
public:
	static FadeScreen* GetInstance();

	void Initialize();
	void Update();
	void Draw();

	void Finalize();

	void FedeIn();
	void FedeOut();

	bool GetIsFadeing() { return isFading; }

private:

	static FadeScreen* instance;

	std::unique_ptr<Sprite> sprite;
	Vector4 color = {1,1,1,1};

	float t = 0.05f;
	//float endt = 1.0f;

	const float deltaTime = 1.0f / 60.0f;

	bool isFading = false;

	FadeScreen() = default;
	~FadeScreen() = default;
	FadeScreen(FadeScreen&) = default;
	FadeScreen& operator=(FadeScreen&) = default;
};