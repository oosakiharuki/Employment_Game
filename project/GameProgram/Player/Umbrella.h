#pragma once
#include "Object3d.h"
#include "MyMath.h"

class Umbrella {
public:
	~Umbrella();
	void Initialize();
	void Update();
	void Draw();

	Vector3 GetTranslate() { return wt.translation_; }
	void SetTranslate(Vector3 translate) { wt.translation_ = translate; }

	void SetRotate(Vector3 rotate) { wt.rotation_ = rotate; }
	AABB GetAABB();

	void ShieldMode(bool isShield) { isShieldMode = isShield; }

private:
	Object3d* object_Open;
	Object3d* object_Close;


	WorldTransform wt;
	AABB umbrellaAABB;

	bool isShieldMode = false;
};

