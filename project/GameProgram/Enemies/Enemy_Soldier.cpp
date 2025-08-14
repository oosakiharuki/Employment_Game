#include "Enemy_Soldier.h"
#include "Input.h"

#include "ImGuiManager.h"

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

	maxHp = 3;
	hp = maxHp;
}

void Enemy_Soldier::Update() {

	if (hp == 0) {
		isDead = true;
	}

	DeadUpdate();

	if (!isDead) {


		if (move.x < route_point1.x) {
			worldTransform.rotation_.y = 90.0f;
		}
		if(move.x > route_point2.x){
			worldTransform.rotation_.y = -90.0f;
		}

		DirectionDegree();

		if (!isFoundTarget) {
			switch (direction)
			{
			case right:
				eyeAABB.min = worldTransform.translation_ + Vector3(0, -2, -1);
				eyeAABB.max = worldTransform.translation_ + Vector3(15, 2, 1);
				speed = { 0.03f,0,0 };
				break;
			case left:
				eyeAABB.min = worldTransform.translation_ + Vector3(-15, -2, -1);
				eyeAABB.max = worldTransform.translation_ + Vector3(0, 2, 1);
				speed = { -0.03f,0,0 };
				break;
			}		
			
			worldTransform.translation_ += speed;		
			move += speed;
		}

		PlayerTarget();

		if (isFoundTarget) {
			Attack();
		}
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


	ImGui::Begin("Enemy_soldier");
	
	//ImGui::Text("translate : %f,%f,%f", worldTransform.translation_.x, worldTransform.translation_.y, worldTransform.translation_.z);


	//ImGui::Text("Eye_Max : %f,%f,%f", eyeAABB.max.x, eyeAABB.max.y, eyeAABB.max.z);
	//ImGui::Text("Eye_Min : %f,%f,%f", eyeAABB.min.x, eyeAABB.min.y, eyeAABB.min.z);

	ImGui::Text("route_point1 : %f,%f,%f", move.x, move.y, move.z);
	ImGui::Text("route_point1 : %f,%f,%f", route_point1.x, route_point1.y, route_point1.z);
	ImGui::Text("route_point2 : %f,%f,%f", route_point2.x, route_point2.y, route_point2.z);

	ImGui::End();

	worldTransform.UpdateMatrix();
}

void Enemy_Soldier::Draw() {
	if (!deleteEnemy) {	
		object->Draw(worldTransform);
	}
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
	
	Vector3 enemyPosition;

	enemyPosition.x = worldTransform.matWorld_.m[3][0];
	enemyPosition.y = worldTransform.matWorld_.m[3][1];
	enemyPosition.z = worldTransform.matWorld_.m[3][2];

	//プレイヤーの方向に向かう(最初に打つ弾にそって進む)
	if (rapidCount == 0) {

		Vector3 playerPosition = player_->GetWorldPosition();

		Vector3 distance = playerPosition - enemyPosition;

		Normalize(distance);

		velocity = Vector3(0.02f, 0.02f, 0.02f) * distance;
	}

	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}