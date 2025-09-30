#include "Enemy_Soldier.h"
#include "Input.h"

#include "ImGuiManager.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Soldier::~Enemy_Soldier() {
	for (auto* bullet : bullets_) {
		delete bullet;
	}
	delete particle_fire;
	delete particle_damage;
	delete shadow_;
}

void Enemy_Soldier::Initialize() {
	wt.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("enemy.obj");

	maxHp = 3;
	hp = maxHp;

	particle_fire = new Particle();
	particle_fire->Initialize("resource/Sprite/cone.png", PrimitiveType::cone);
	particle_fire->SetParticleCount(1);
	particle_fire->ChangeMode(BornParticle::Stop);
	particle_fire->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.1f);


	particle_damage = new Particle();
	particle_damage->Initialize("resource/Sprite/circle.png", PrimitiveType::ring);
	particle_damage->SetParticleCount(10);
	particle_damage->ChangeMode(BornParticle::Stop);
	particle_damage->SetParticleMosion(ParticleMosion::Exprosion);
	particle_damage->SetFrequency(1.0f);

	///影
	shadow_ = new Shadow();
	shadow_->Initialize();
	shadow_->SetScale({ 1,0,1 });

}

void Enemy_Soldier::Update() {

	if (hp == 0) {
		isDead = true;
	}

	DeadUpdate();

	if (!isDead) {

		//重力
		GrabityUpdate();


		if (move.x < route_point1.x) {
			wt.rotation_.y = 90.0f;
		}
		if(move.x > route_point2.x){
			wt.rotation_.y = -90.0f;
		}

		DirectionDegree();

		if (!isFoundTarget && !isBulletStart) {
			switch (direction)
			{
			case right:
				eyeAABB.min = wt.translation_ + Vector3(0, -10, -1);
				eyeAABB.max = wt.translation_ + Vector3(20, 10, 1);
				speed = { 0.03f,0,0 };
				break;
			case left:
				eyeAABB.min = wt.translation_ + Vector3(-20, -10, -1);
				eyeAABB.max = wt.translation_ + Vector3(0, 10, 1);
				speed = { -0.03f,0,0 };
				break;
			}		
			
			wt.translation_ += speed;		
			move += speed;
		}

		PlayerTarget();

		if (isFoundTarget) {
			isBulletStart = true;
		}
		if (isBulletStart) {
			Attack();
		}
		else {
			rapidCount = 0;
			rapidFireTime = 0;
			coolTime = 0;
		}

		shadow_->SetTranslate(wt.translation_);
	}

	if (isDamageMosion) {
		ScaleUpdate(&isDamageMosion, damageScale, damageMaxTime);
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


	particle_fire->SetRotate({ 0,0,-wt.rotation_.y });
	particle_fire->Update();

	particle_damage->Update();

#ifdef _DEBUG

	ImGui::Begin("Enemy_soldier");
	
	//ImGui::Text("translate : %f,%f,%f", wt.translation_.x, wt.translation_.y, wt.translation_.z);


	//ImGui::Text("Eye_Max : %f,%f,%f", eyeAABB.max.x, eyeAABB.max.y, eyeAABB.max.z);
	//ImGui::Text("Eye_Min : %f,%f,%f", eyeAABB.min.x, eyeAABB.min.y, eyeAABB.min.z);

	ImGui::Text("route_point1 : %f,%f,%f", move.x, move.y, move.z);
	ImGui::Text("route_point1 : %f,%f,%f", route_point1.x, route_point1.y, route_point1.z);
	ImGui::Text("route_point2 : %f,%f,%f", route_point2.x, route_point2.y, route_point2.z);

	ImGui::End();

#endif // _DEBUG

	object->Update(wt);
	wt.UpdateMatrix();
}

void Enemy_Soldier::Draw() {
	if (!deleteEnemy) {
		object->Draw();
		shadow_->Draw();
	}
	for (auto* bullet : bullets_) {
		bullet->Draw();
	}


	ParticleCommon::GetInstance()->Command();

	particle_fire->Draw();
	particle_damage->Draw();

	Object3dCommon::GetInstance()->Command();
	
}



void Enemy_Soldier::IsDamage() {
	particle_damage->SetTranslate(wt.translation_);
	particle_damage->ChangeMode(BornParticle::MomentMode);
	isDamageMosion = true;
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
			particle_fire->ChangeMode(BornParticle::MomentMode);
			rapidCount++;
			rapidFireTime = 0;
		}

		if (rapidCount == rapidFireMax) {
			rapidCount = 0;
			coolTime = 0;
			isBulletStart = false;
		}
	}


}

void Enemy_Soldier::Fire() {
	
	Vector3 enemyPosition;

	enemyPosition.x = wt.matWorld_.m[3][0];
	enemyPosition.y = wt.matWorld_.m[3][1];
	enemyPosition.z = wt.matWorld_.m[3][2];

	//プレイヤーの方向に向かう(最初に打つ弾にそって進む)
	if (rapidCount == 0) {

		const float kSpeed = 0.4f;

		Vector3 playerPosition = player_->GetWorldPosition();

		Vector3 distance = playerPosition - enemyPosition;

		Vector3 normal = Normalize(distance);

		normal.x *= kSpeed;
		normal.y *= kSpeed;
		normal.z *= kSpeed;

		velocity = normal;
	}
	particle_fire->SetTranslate(enemyPosition);

	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}

void Enemy_Soldier::RespownEnemy() {
	RespownEnemy_All();
	move = { 0,0,0 };
}