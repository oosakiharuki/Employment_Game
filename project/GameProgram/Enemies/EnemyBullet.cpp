#include "EnemyBullet.h"

using namespace MyMath;

EnemyBullet::~EnemyBullet() {}


void EnemyBullet::Initialize() {
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModelFile("PlayerBullet.obj");

	wt.Initialize();

	bulletAABB.min = { -0.5f,-0.5f,-0.5f };
	bulletAABB.max = { 0.5f,0.5f,0.5f };
}

void EnemyBullet::Update() {
	if (!isPari) {
		wt.translation_ += velocity_;
	}
	else {
		///パリィされた時
		wt.translation_ -= velocity_;
	}


	deathTimer += 1.0f / 60.0f;

	if (deathTimer >= endTime) {
		isDead = true;
	}

	object->Update(wt);
	wt.UpdateMatrix();
}

void EnemyBullet::Draw() {
	object->Draw();
}

AABB EnemyBullet::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + bulletAABB.min;
	aabb.max = wt.translation_ + bulletAABB.max;
	return aabb;
}

void EnemyBullet::Pari_Mode() {
	isPari = true;
}
