#pragma once
#include "MyMath.h"
class BaseScene;
class Player;
class CameraControl;

namespace StageObjectFunction {

	void WarpNextScene(Player& player, CameraControl* cameraControl_, bool& isNextScene);

	//ズームするときのプレイヤーと少し離れた位置
	const Vector3 kPlayerAwayPos_ = { 0, 2, -15.0f };
};

