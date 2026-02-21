#pragma once
#include "MyMath.h"
#include "CollisionSource.h"


class EnemyEye : public CollisionSource {
public:
	void Update(const Vector3& enemyPos,const Vector3& foundReach);

	bool IsFound() { return isFound_; }

	void OnCollision(CollisionSource* collision) override;

private:

	//発見フラグ
	bool isFound_ = false;

	bool isWall_ = false;

	Vector3 enemyPosition_{};
	Vector3 playerPosition_{};
};

