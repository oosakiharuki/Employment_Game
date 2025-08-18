#include "Enemy_Bomb.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Bomb::~Enemy_Bomb() {
	delete particle_Bom;
}

void Enemy_Bomb::Initialize() {
	worldTransform.Initialize();

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
}

void Enemy_Bomb::Update() {

	if (hp == 0) {
		isDead = true;
		bombAABB.min = { 0,-10,0 };
		bombAABB.max = { 0,-10,0 };
	}

	DeadUpdate();

	if (!isDead) {

		if (!isStart) {
			if (move.x < route_point1.x) {
				worldTransform.rotation_.y = 90.0f;
			}
			if (move.x > route_point2.x) {
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
		}	
		
		PlayerTarget();

		if (isFoundTarget || isStart) {
			Attack();
		}
	}

	particle_Bom->Update();

	worldTransform.UpdateMatrix();
}

void Enemy_Bomb::Draw() {
	if (!isDead) {
		object->Draw(worldTransform);
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

	Vector3 distance = enemyPosition - playerPosition;

	distance = Normalize(distance);

	worldTransform.translation_ += distance * speed * 3;

	if (distance.x < 0) {
		worldTransform.rotation_.y = 90.0f;
	}
	if (distance.x >= 0) {
		worldTransform.rotation_.y = -90.0f;
	}

	if (bombTimer >= bombTimeMax) {
		Exprosion();
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
	isDead = false;
	deleteEnemy = false;
	isStart = false;
	bombTimer = 0.0f;
	hp = maxHp;
	//blenderで配置した初期位置に戻る
	worldTransform.translation_ = init_point;
	worldTransform.rotation_ = init_rotate;

	move = { 0,0,0 };

	bombAABB.min = { 0,-10,0 };
	bombAABB.max = { 0,-10,0 };
}

Vector3 Enemy_Bomb::GetWorldPosition() {
	Vector3 result;

	result.x = worldTransform.matWorld_.m[3][0];
	result.y = worldTransform.matWorld_.m[3][1];
	result.z = worldTransform.matWorld_.m[3][2];

	return result;
}

void Enemy_Bomb::Exprosion() {
	bombAABB.min = worldTransform.translation_ - hani;
	bombAABB.max = worldTransform.translation_ + hani;

	particle_Bom->SetTranslate(worldTransform.translation_);
	particle_Bom->ChangeMode(BornParticle::MomentMode);

	IsDamage();
}