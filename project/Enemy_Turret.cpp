#include "Enemy_Turret.h"

using namespace MyMath;

Enemy_Turret::~Enemy_Turret() {
	for (auto* bullet : bullets_) {
		delete bullet;
	}
}


void Enemy_Turret::Initialize() {
	worldTransform.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("cannon.obj");
}

void Enemy_Turret::Update() {

	if (hp == 0) {
		isDead = true;
	}

	if (isDead) {
		worldTransform.translation_.y += 0.1f;
	}

	Attack();

	for (auto* bullet : bullets_) {
		bullet->Update();
	}

	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	if (worldTransform.translation_.y >= 10.0f) {
		isDead = false;
		hp = maxHp;
		worldTransform.translation_.y = 0.0f;
	}

	worldTransform.UpdateMatrix();
}

void Enemy_Turret::Draw() {
	object->Draw(worldTransform);
	for (auto* bullet : bullets_) {
		bullet->Draw();
	}
}



void Enemy_Turret::IsDamage() {
	if (hp == 0) {
		return;
	}
	hp -= 1;
}

void Enemy_Turret::Attack() {

	coolTime += 1.0f / 60.0f;
	if (coolTime >= 2.0f) {
		ShootBullet();
		coolTime = 0;
	}

}

void Enemy_Turret::ShootBullet() {
	Vector3 translate = {
	worldTransform.translation_.x,
	worldTransform.translation_.y + 1.0f,
	worldTransform.translation_.z
	};

	Vector3 velocity = { 0.0f,0.0f,0.5f };
	velocity = TransformNormal(velocity, worldTransform.matWorld_);

	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetTranslate(translate);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}
