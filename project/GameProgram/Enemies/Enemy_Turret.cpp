#include "Enemy_Turret.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Turret::~Enemy_Turret() {
	for (auto* bullet : bullets_) {
		delete bullet;
	}
	delete particle_damage;
	delete particle_dead;
	delete particle_fire;
}


void Enemy_Turret::Initialize() {
	worldTransform.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("cannon.obj");

	maxHp = 6;
	hp = maxHp;

	particle_fire = new Particle();
	particle_fire->Initialize("resource/Sprite/cone.png", PrimitiveType::cone);
	particle_fire->SetParticleCount(1);
	particle_fire->ChangeMode(BornParticle::Stop);
	particle_fire->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.1f);


	particle_dead = new Particle();
	particle_dead->Initialize("resource/Sprite/gradationLine.png", PrimitiveType::ring);
	particle_dead->SetParticleCount(1);
	particle_dead->SetParticleMosion(ParticleMosion::Normal);
	particle_dead->ChangeMode(BornParticle::Stop);

	particle_damage = new Particle();
	particle_damage->Initialize("resource/Sprite/circle.png", PrimitiveType::ring);
	particle_damage->SetParticleCount(10);
	particle_damage->ChangeMode(BornParticle::Stop);
	particle_damage->SetParticleMosion(ParticleMosion::Exprosion);
	particle_damage->SetFrequency(1.0f);

}

void Enemy_Turret::Update() {

	if (hp == 0) {
		isDead = true;
		//particle_dead->SetTranslate(worldTransform.translation_);
		//particle_dead->ChangeMode(BornParticle::MomentMode);
	}

	DeadUpdate();

	if (!isDead) {

		//重力
		GrabityUpdate();

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


	particle_fire->SetScale({2,2,2});
	particle_fire->SetRotate({0,0,-worldTransform.rotation_.y});


	particle_fire->Update();

	particle_damage->Update();
	particle_dead->Update();

#ifdef _DEBUG

	ImGui::Begin("Enemy_Turret");

	ImGui::Text("translate : %f,%f,%f", worldTransform.translation_.x, worldTransform.translation_.y, worldTransform.translation_.z);
	ImGui::Text("translate : %f,%f,%f", worldTransform.rotation_.x, worldTransform.rotation_.y, worldTransform.rotation_.z);
	
	ImGui::Text("Eye_Min : %f,%f,%f", eyeAABB.min.x, eyeAABB.min.y, eyeAABB.min.z);
	ImGui::Text("Eye_Max : %f,%f,%f", eyeAABB.max.x, eyeAABB.max.y, eyeAABB.max.z);



	ImGui::End();

#endif // _DEBUG

	worldTransform.UpdateMatrix();
}

void Enemy_Turret::Draw() {

	if (!deleteEnemy) {
		object->Draw(worldTransform);
	}

	for (auto* bullet : bullets_) {
		bullet->Draw();
	}

	ParticleCommon::GetInstance()->Command();

	particle_fire->Draw();
	particle_dead->Draw();
	particle_damage->Draw();

	Object3dCommon::GetInstance()->Command();

}



void Enemy_Turret::IsDamage() {
	particle_damage->SetTranslate(worldTransform.translation_);
	particle_damage->ChangeMode(BornParticle::MomentMode);

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

void Enemy_Turret::Fire() {
	Vector3 translate = {
	worldTransform.translation_.x - 1.0f,
	worldTransform.translation_.y + 1.0f,
	worldTransform.translation_.z
	};


	particle_fire->SetTranslate({ translate.x + 3.0f,translate.y,translate.z });


	Vector3 velocity = { 0.0f,0.0f,0.5f };
	velocity = TransformNormal(velocity, worldTransform.matWorld_);

	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetTranslate(translate);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}

void Enemy_Turret::RespownEnemy() {
	RespownEnemy_All();
}