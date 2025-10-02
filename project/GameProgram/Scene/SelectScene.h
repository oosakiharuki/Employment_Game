#pragma once
#include "IScene.h"

class SelectScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:

	//カメラズーム
	Segment cameraSegment{};
	float zumuTimer = 0.0f;

	bool isZumuIn = false;
};