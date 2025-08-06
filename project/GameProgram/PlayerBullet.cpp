#include "PlayerBullet.h"

using namespace MyMath;

PlayerBullet::~PlayerBullet() {
	delete object;
}


void PlayerBullet::Initialize() {
	object = new Object3d();
	object->Initialize();
	object->SetModelFile("PlayerBullet.obj");

	worldTransform_.Initialize();

	bulletAABB.min = { -1,-1,-1 };
	bulletAABB.max = { 1,1,1 };
}

void PlayerBullet::Update() {
	worldTransform_.translation_ += velocity_;

	deathTimer += 1.0f / 60.0f;
	
	if (deathTimer >= endTime) {
		isDead = true;
	}


	worldTransform_.UpdateMatrix();
}

void PlayerBullet::Draw() {
	object->Draw(worldTransform_);
}

AABB PlayerBullet::GetAABB() {
	AABB aabb;
	aabb.min = worldTransform_.translation_ + bulletAABB.min;
	aabb.max = worldTransform_.translation_ + bulletAABB.max;
	return aabb;
}
