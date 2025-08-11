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

void IEnemy::PlayerTarget() {

	if (IsCollisionAABB(player_->GetAABB(), eyeAABB)) {
		isFoundTarget = true;
	}
	else {
		isFoundTarget = false;
	}

}


void IEnemy::DeadUpdate() {
	if (isDead) {
		worldTransform.translation_.y += 0.1f;
	}

	if (worldTransform.translation_.y >= 10.0f) {
		deleteEnemy = true;
		worldTransform.translation_.y = 0.0f;
	}
}
