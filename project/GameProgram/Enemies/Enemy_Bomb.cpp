#include "Enemy_Bomb.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Bomb::~Enemy_Bomb() {}

void Enemy_Bomb::Initialize() {
	//ワールド座標系の初期化
	wt.Initialize();

	//オブジェクトの初期化
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModelFile("sphere.obj");

	//体力の初期化
	maxHp = 1;
	hp = maxHp;

	particle_damage = std::make_unique<Particle>();
	particle_damage->Initialize("enemySoldier_damage", "resource/Sprite/circle.png", PrimitiveType::ring);
	particle_damage->SetParticleCount(10);
	particle_damage->ChangeMode(BornParticle::Stop);
	particle_damage->SetParticleMosion(ParticleMosion::Exprosion);
	particle_damage->SetFrequency(1.0f);

	///影
	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();
	shadow_->SetScale({ 1,0,1 });

	//見える範囲初期化
	eyeReach = { 15, 2, 1 };
}

void Enemy_Bomb::Update() {
	
	if (isDead) {
		isExplosion = true;
	}

	//敵の共有処理
	UpdateCommon();

	//体力が0の時
	if ((bombTimer >= bombTimeMax || isDead) && !isExplosion) {
		Exprosion();
	}

	if (!isDead) {

		if (!isStart) {
			MoveEnemy();

			if (!isFoundTarget) {
				SearchRange();
			}
		}	
		else {
			TimeRimmit();
		}
	}

	//更新が終了
	UpdateBehind();
}

void Enemy_Bomb::Draw() {
	if (!isDead) {
		object->Draw();
		shadow_->Draw();
	}

	ParticleCommon::GetInstance()->Command();

	particle_damage->Draw();

	Object3dCommon::GetInstance()->Command();
}

void Enemy_Bomb::Attack() {
	isStart = true;
}

void Enemy_Bomb::TimeRimmit() {

	bombTimer += deltaTime;

	Vector3 enemyPosition = GetWorldPosition();
	Vector3 playerPosition = player_->GetWorldPosition();

	distance = enemyPosition - playerPosition;

	distance = Normalize(distance);

	wt.translation_ += distance * Vector3{ -0.03f,0,0 } *3;

	if (distance.x < 0) {
		wt.rotation_.y = 90.0f;
	}
	if (distance.x >= 0) {
		wt.rotation_.y = -90.0f;
	}

	bool s = true;
	if (bombTimer >= bombTimeMax / 1.5f) {
		//爆発寸前だと揺れが細かくなる
		ScaleUpdate(&s, Vector3(0.05f, 0.05f, 0.05f) * 2, 0.2f / 2);
	}
	else {
		ScaleUpdate(&s, { 0.05f,0.05f,0.05f }, 0.2f);
	}
}

void Enemy_Bomb::RespawnEnemy() {
	RespawnEnemyCommon();
	
	isStart = false;
	isExplosion = false;

	bombTimer = 0.0f;
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

	particle_damage->SetTranslate(wt.translation_);
	particle_damage->ChangeMode(BornParticle::MomentMode);

	isDead = true;
}