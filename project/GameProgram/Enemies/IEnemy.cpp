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


	worldTransform.rotation_.y = std::fmod(worldTransform.rotation_.y, 360.0f);


	if (worldTransform.rotation_.y >= 0.0f && worldTransform.rotation_.y < 180.0f) {
		direction = left;
	}
	else if (worldTransform.rotation_.y >= 180.0f && worldTransform.rotation_.y < 360.0f) {
		direction = right;
	}

	if (IsCollisionAABB(player_->GetAABB(), eyeAABB)) {
		isFoundTarget = true;
	}
	else {
		isFoundTarget = false;
	}

}


void IEnemy::DeadUpdate() {
	if (isDead) {
		worldTransform.rotation_.z += 3.0f;
	}

	if (worldTransform.rotation_.z > 90.0f) {
		deleteEnemy = true;
	}
}

void IEnemy::DirectionDegree() {

	//0~360にする
	worldTransform.rotation_.y = std::fmod(worldTransform.rotation_.y, 360.0f);
	//-の場合
	if (worldTransform.rotation_.y < 0)
		worldTransform.rotation_.y += 360.0;


	///0~180は右
	if (worldTransform.rotation_.y >= 0.0f && worldTransform.rotation_.y < 180.0f) {
		direction = right;
	}///180~360は右
	else if (worldTransform.rotation_.y <= 360.0f) {
		direction = left;
	}
}