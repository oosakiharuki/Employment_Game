#include "IEnemy.h"

using namespace MyMath;
IEnemy::IEnemy() {}

IEnemy::~IEnemy(){
	delete object;
}


void IEnemy::GrabityUpdate() {
	
	grabity -= 0.01f;
	//重力
	if (!isGround) {
		worldTransform.translation_.y += grabity;
	}
	else {
		grabity = 0.0f;
	}

}

AABB IEnemy::GetAABB() {
	AABB aabb;
	aabb.min = worldTransform.translation_ + enemyAABB.min;
	aabb.max = worldTransform.translation_ + enemyAABB.max;
	return aabb;
}



void IEnemy::PlayerTarget() {

	if (IsCollisionAABB(player_->GetAABB(), eyeAABB) && !player_->GetIsPlayerDown()) {
		isFoundTarget = true;
	}
	else {
		isFoundTarget = false;
	}

}

void IEnemy::RespownEnemy_All() {
	isDead = false;
	deleteEnemy = false;
	hp = maxHp;

	//blenderで配置した初期位置に戻る
	worldTransform.translation_ = init_point;
	worldTransform.rotation_ = init_rotate;
}

void IEnemy::DeadUpdate() {
	if (isDead) {
		worldTransform.rotation_.z += 3.0f;
	}
	else {
		worldTransform.rotation_.z = 0.0f;
		return;
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
