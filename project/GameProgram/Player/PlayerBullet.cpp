#include "PlayerBullet.h"

using namespace MyMath;

PlayerBullet::~PlayerBullet() {
	delete object;
}


void PlayerBullet::Initialize() {
	object = new Object3d();
	object->Initialize();
	object->SetModelFile("PlayerBullet.obj");

	wt.Initialize();

	bulletAABB.min = { -1,-1,-1 };
	bulletAABB.max = { 1,1,1 };
}

void PlayerBullet::Update() {

	deathTimer += 1.0f / 60.0f;
	
	Vector3 speed;
	speed = EaseIn(velocity_,deathTimer,endTime);

	wt.translation_ += speed;

	if (deathTimer >= endTime) {
		isDead = true;
	}

	object->Update(wt);
	wt.UpdateMatrix();
}

void PlayerBullet::Draw() {
	object->Draw();
}

AABB PlayerBullet::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + bulletAABB.min;
	aabb.max = wt.translation_ + bulletAABB.max;
	return aabb;
}
