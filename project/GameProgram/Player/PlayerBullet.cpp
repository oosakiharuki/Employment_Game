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

	deathTimer += 1.0f / 60.0f;
	
	Vector3 speed;
	speed = EaseIn(velocity_,deathTimer,endTime);

	worldTransform_.translation_ += speed;

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
