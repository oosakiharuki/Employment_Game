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

	if (isHit) {
		worldTransform.translation_.y += 0.1f;
	}

	if (worldTransform.translation_.y >= 10.0f) {
		isHit = false;
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