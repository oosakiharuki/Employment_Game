#include "EnemyBullet.h"

using namespace MyMath;

EnemyBullet::~EnemyBullet() {
	delete object;
}


void EnemyBullet::Initialize() {
	object = new Object3d();
	object->Initialize();
	object->SetModelFile("PlayerBullet.obj");

	worldTransform_.Initialize();

	bulletAABB.min = { -1,-1,-1 };
	bulletAABB.max = { 1,1,1 };
}

void EnemyBullet::Update() {
	if (!isPari) {
		worldTransform_.translation_ += velocity_;
	}
	else {
		///パリィされた時
		worldTransform_.translation_ -= velocity_;
	}


	deathTimer += 1.0f / 60.0f;

	if (deathTimer >= endTime) {
		isDead = true;
	}


	worldTransform_.UpdateMatrix();
}

void EnemyBullet::Draw() {
	object->Draw(worldTransform_);
}

AABB EnemyBullet::GetAABB() {
	AABB aabb;
	aabb.min = worldTransform_.translation_ + bulletAABB.min;
	aabb.max = worldTransform_.translation_ + bulletAABB.max;
	return aabb;
}

void EnemyBullet::Pari_Mode() {
	isPari = true;
}
