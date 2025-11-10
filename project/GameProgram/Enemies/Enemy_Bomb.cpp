#include "Enemy_Bomb.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Bomb::~Enemy_Bomb() {}

void Enemy_Bomb::Initialize() {

	Enemy_InitializeCommon();
	object->SetModelFile("enemy_bomb.obj");

	//体力の初期化
	HP_Initialize(1);

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
		ScaleUpdate(&s, bombScale * 2, 0.2f / 2);
	}
	else {
		ScaleUpdate(&s, bombScale, 0.2f);
	}
}

void Enemy_Bomb::RespawnEnemy() {
	RespawnEnemyCommon();
	
	isStart = false;
	isExplosion = false;

	bombTimer = 0.0f;
}

void Enemy_Bomb::Exprosion() {
	bombAABB.min = wt.translation_ - hani;
	bombAABB.max = wt.translation_ + hani;

	particle_damage->SetTranslate(wt.translation_);
	particle_damage->ChangeMode(BornParticle::MomentMode);

	isDead = true;
}