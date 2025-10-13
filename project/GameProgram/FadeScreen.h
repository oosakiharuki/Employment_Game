#pragma once
#include "Sprite.h"
#include "Dissolve.h"

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

	const float deltaTime = 1.0f / 60.0f;

	bool isFading = false;
	float degress = 0.0f;

	std::unique_ptr<IPostEffects> postEffect_ = nullptr;
	Dissolve* dissolve;

	FadeScreen() = default;
	~FadeScreen() = default;
	FadeScreen(FadeScreen&) = default;
	FadeScreen& operator=(FadeScreen&) = default;
};