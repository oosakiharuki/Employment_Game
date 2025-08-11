#include "Enemy_Turret.h"
#include "ImGuiManager.h"

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

	DeadUpdate();

	DirectionDegree();


	switch (direction)
	{
	case right:
		eyeAABB.min = worldTransform.translation_ + Vector3(0, -2, -1);
		eyeAABB.max = worldTransform.translation_ + Vector3(15, 2, 1);
		break;
	case left:
		eyeAABB.min = worldTransform.translation_ + Vector3(-15, -2, -1);
		eyeAABB.max = worldTransform.translation_ + Vector3(0, 2, 1);
		break;
	}

	PlayerTarget();

	if (isFoundTarget) {
		Attack();
	}
	else {
		rapidCount = 0;
		rapidFireTime = 0;
		coolTime = 0;
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


	ImGui::Begin("Enemy_Turret");

	ImGui::Text("translate : %f,%f,%f", worldTransform.translation_.x, worldTransform.translation_.y, worldTransform.translation_.z);
	
	ImGui::Text("Eye_Min : %f,%f,%f", eyeAABB.min.x, eyeAABB.min.y, eyeAABB.min.z);
	ImGui::Text("Eye_Max : %f,%f,%f", eyeAABB.max.x, eyeAABB.max.y, eyeAABB.max.z);



	ImGui::End();

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

void Enemy_Turret::Fire() {
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
