#pragma once
#include "Object3d.h"
#include "MyMath.h"

class Umbrella {
public:
	~Umbrella();
	void Initialize();
	void Update();
	void Draw();

	void SetTranslate(Vector3 translate) { wt.translation_ = translate; }

	void SetRotate(Vector3 rotate) { wt.rotation_ = rotate; }
	AABB GetAABB();

private:
	Object3d* object_;
	WorldTransform wt;
	AABB umbrellaAABB;
};

