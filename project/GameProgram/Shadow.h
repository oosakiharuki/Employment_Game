#pragma once
#include "Object3d.h"
#include "Object_glTF.h"
#include "MyMath.h"

class Shadow {
public:
	~Shadow();
	void Initialize();
	void Update();
	void Draw();

	void SetTranslate(Vector3 position) { wt.translation_ = position; }
	void SetScale(Vector3 scale) { wt.scale_ = scale; }

	AABB GetAABB();

private:
	Object3d* object_;
	WorldTransform wt;
	AABB shadowAABB;
};