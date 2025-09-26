#include "Enemy_Bomb.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Bomb::~Enemy_Bomb() {
	delete particle_Bom;
	delete shadow_;
}

void Enemy_Bomb::Initialize() {
	wt.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("sphere.obj");

	maxHp = 1;
	hp = maxHp;

	particle_Bom = new Particle();
	particle_Bom->Initialize("resource/Sprite/circle.png", PrimitiveType::ring);
	particle_Bom->SetParticleCount(10);
	particle_Bom->ChangeMode(BornParticle::Stop);
	particle_Bom->SetParticleMosion(ParticleMosion::Exprosion);
	particle_Bom->SetFrequency(1.0f);

	bombAABB.min = { 0,-10,0 };
	bombAABB.max = { 0,-10,0 };

	///影
	shadow_ = new Shadow();
	shadow_->Initialize();
	shadow_->SetScale({ 1,0,1 });
}

void Enemy_Bomb::Update() {

	if (hp == 0) {
		isDead = true;
		bombAABB.min = { 0,-10,0 };
		bombAABB.max = { 0,-10,0 };
	}

	DeadUpdate();

	if (!isDead) {
		
		//重力
		GrabityUpdate();

		if (!isStart) {
			if (move.x < route_point1.x) {
				wt.rotation_.y = 90.0f;
			}
			if (move.x > route_point2.x) {
				wt.rotation_.y = -90.0f;
			}

			DirectionDegree();

			if (!isFoundTarget) {
				switch (direction)
				{
				case right:
					eyeAABB.min = wt.translation_ + Vector3(0, -2, -1);
					eyeAABB.max = wt.translation_ + Vector3(15, 2, 1);
					speed = { 0.03f,0,0 };
					break;
				case left:
					eyeAABB.min = wt.translation_ + Vector3(-15, -2, -1);
					eyeAABB.max = wt.translation_ + Vector3(0, 2, 1);
					speed = { -0.03f,0,0 };
					break;
				}

				wt.translation_ += speed;
				move += speed;
			}
		}	
		
		PlayerTarget();

		if (isFoundTarget || isStart) {
			Attack();
		}

		Vector3 shadowPosition = wt.translation_;
		shadowPosition.y -= 1.0f;

		shadow_->SetTranslate(shadowPosition);
		shadow_->Update();
	}

	particle_Bom->Update();

	object->Update(wt);
	wt.UpdateMatrix();
}

void Enemy_Bomb::Draw() {
	if (!isDead) {
		object->Draw();
		shadow_->Draw();
	}

	ParticleCommon::GetInstance()->Command();
	particle_Bom->Draw();
	Object3dCommon::GetInstance()->Command();
}

void Enemy_Bomb::Attack() {

	isStart = true;

	bombTimer += deltaTime;
	
	Vector3 enemyPosition = GetWorldPosition();
	Vector3 playerPosition = player_->GetWorldPosition();

	distance = enemyPosition - playerPosition;

	distance = Normalize(distance);

	wt.translation_ += distance * Vector3{ -0.03f,0,0 } * 3;

	if (distance.x < 0) {
		wt.rotation_.y = 90.0f;
	}
	if (distance.x >= 0) {
		wt.rotation_.y = -90.0f;
	}

	if (bombTimer >= bombTimeMax) {
		Exprosion();
	}
	
	bool s = true;
	if (bombTimer >= bombTimeMax / 1.5f) {
		//爆発寸前だと揺れが細かくなる
		ScaleUpdate(&s, Vector3(0.05f,0.05f,0.05f) * 2, 0.1f / 2);
	}
	else {
		ScaleUpdate(&s, { 0.05f,0.05f,0.05f }, 0.1f);
	}
}

void Enemy_Bomb::IsDamage() {
	if (hp == 0) {
		return;
	}
	hp -= 1;
	Exprosion();
}

void Enemy_Bomb::RespownEnemy() {
	RespownEnemy_All();
	
	isStart = false;
	bombTimer = 0.0f;
	move = { 0,0,0 };

	bombAABB.min = { 0,-10,0 };
	bombAABB.max = { 0,-10,0 };
}

Vector3 Enemy_Bomb::GetWorldPosition() {
	Vector3 result;

	result.x = wt.matWorld_.m[3][0];
	result.y = wt.matWorld_.m[3][1];
	result.z = wt.matWorld_.m[3][2];

	return result;
}

void Enemy_Bomb::Exprosion() {
	bombAABB.min = wt.translation_ - hani;
	bombAABB.max = wt.translation_ + hani;

	particle_Bom->SetTranslate(wt.translation_);
	particle_Bom->ChangeMode(BornParticle::MomentMode);

	IsDamage();
}