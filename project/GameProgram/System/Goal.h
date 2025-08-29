#pragma once
#include "Object3d.h"
#include "MyMath.h"

class Goal {
public:

	~Goal();
	void Initialize();
	void Update();
	void Draw();

	Vector3 GetPosition() { return wt.translation_; }
	void SetPosition(Vector3 position) { wt.translation_ = position; }

	AABB GetAABB();
	void SetAABB(AABB aabb) { goalAABB = aabb; }

private:
	Object3d* object_;
	WorldTransform wt;

	AABB goalAABB;

};