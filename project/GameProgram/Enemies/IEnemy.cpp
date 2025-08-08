#include "IEnemy.h"

using namespace MyMath;
IEnemy::IEnemy() {}

IEnemy::~IEnemy(){
	delete object;
}

AABB IEnemy::GetAABB() {
	AABB aabb;
	aabb.min = worldTransform.translation_ + enemyAABB.min;
	aabb.max = worldTransform.translation_ + enemyAABB.max;
	return aabb;
}