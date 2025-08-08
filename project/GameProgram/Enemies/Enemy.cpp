#include "Enemy.h"
#include "Input.h"

using namespace MyMath;

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



void Enemy::IsDamage() { 
	if (hp == 0) {
		return;
	}
	hp -= 1;
}

void Enemy::Attack() {

}