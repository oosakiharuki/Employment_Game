#include "Enemy.h"
#include "Input.h"

using namespace MyMath;

Enemy::Enemy() {}

Enemy::~Enemy() {
	delete object;
}

void Enemy::Initialize() {
	worldTransform.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("enemy.obj");
}

void Enemy::Update() {

	if (hp == 0) {
		isDead = true;
	}

	if (isDead) {
		worldTransform.translation_.y += 0.1f;
	}

	if (worldTransform.translation_.y >= 10.0f) {
		isDead = false;
		hp = maxHp;
		worldTransform.translation_.y = 0.0f;
	}


	worldTransform.rotation_.y += 1.0f;
	worldTransform.UpdateMatrix();
}

void Enemy::Draw() {
	object->Draw(worldTransform);
}

AABB Enemy::GetAABB() {
	AABB aabb;
	aabb.min = worldTransform.translation_ + enemyAABB.min;
	aabb.max = worldTransform.translation_ + enemyAABB.max;
	return aabb;
}

void Enemy::IsDamage() { 
	if (hp == 0) {
		return;
	}
	hp -= 1;
}