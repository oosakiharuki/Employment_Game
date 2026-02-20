#pragma once
#include "MyMath.h"

enum CollisionTypes {
	player,
	playerBullet,
	umbrella,
	enemy,
	enemyEye,
	enemyBullet,
	parryBullet,
	stageObject,
	shadow,
	stage,
	boss,
	bombExplotion,
	event,
};

class CollisionSource {
public:

	AABB GetAABB() { return collisionAABB_; }
	void SetAABB(const AABB& aabb) { collisionAABB_ = aabb; }

	CollisionTypes GetType() { return collisionType_; }
	void SetType(const CollisionTypes& type) { collisionType_ = type; }

	const Vector3& GetCenter() { return center_; }
	void SetCenter(const Vector3& center) { center_ = center; }

	Vector3 GetDistance(const Vector3& otherCenter);

	virtual void OnCollision(CollisionSource* collision);

protected:

	CollisionTypes collisionType_;
	AABB collisionAABB_;
	Vector3 center_;
};


