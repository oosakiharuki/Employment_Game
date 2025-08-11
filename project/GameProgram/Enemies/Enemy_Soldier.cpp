#include "Enemy_Soldier.h"
#include "Input.h"

using namespace MyMath;

Enemy_Soldier::~Enemy_Soldier() {
	for (auto* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy_Soldier::Initialize() {
	worldTransform.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("enemy.obj");
}

void Enemy_Soldier::Update() {

	if (hp == 0) {
		isDead = true;
	}

	DeadUpdate();
	

	eyeAABB.min = worldTransform.translation_ + Vector3(-6, -2, -1);
	eyeAABB.max = worldTransform.translation_ + Vector3(0, 2, 1);
	
	PlayerTarget();

	if (isFoundTarget) {
		Attack();
	}
	
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

	worldTransform.UpdateMatrix();
}

void Enemy_Soldier::Draw() {
	object->Draw(worldTransform);
	for (auto* bullet : bullets_) {
		bullet->Draw();
	}
}



void Enemy_Soldier::IsDamage() {
	if (hp == 0) {
		return;
	}
	hp -= 1;
}

void Enemy_Soldier::Attack() {

	coolTime += 1.0f / 60.0f;
	if (coolTime >= coolTimeMax) {

		rapidFireTime += 1.0f / 60.0f;

		if (rapidFireTime >= rapidFireTimeMax) {
			Fire();
			rapidCount++;
			rapidFireTime = 0;
		}

		if (rapidCount == rapidFireMax) {
			rapidCount = 0;
			coolTime = 0;
		}
	}


}

void Enemy_Soldier::Fire() {
	Vector3 translate = {
	worldTransform.translation_.x,
	worldTransform.translation_.y,
	worldTransform.translation_.z
	};

	Vector3 velocity =  Vector3(0.0f,0.0f,0.5f);
	velocity = TransformNormal(velocity, worldTransform.matWorld_);

	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetTranslate(translate);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}