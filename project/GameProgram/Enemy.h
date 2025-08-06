#pragma once
#include "MyMath.h"
#include "Object3d.h"

class Enemy {
public:
	Enemy();
	~Enemy();
	void Initialize();
	void Update();
	void Draw();

	void SetTranslate(Vector3 translate) { worldTransform.translation_ = translate; }
	void SetRotate(Vector3 rotate) { worldTransform.rotation_ = rotate; }
	
	void SetAABB(AABB aabb) { enemyAABB = aabb; }
	AABB GetAABB();

	void IsHit() { isHit = true; }

private:
	Object3d* object;
	WorldTransform worldTransform;
	AABB enemyAABB;
	
	bool isHit = false;
};