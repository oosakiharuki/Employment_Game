#include "Enemy_Soldier.h"
#include "Input.h"

#include "ImGuiManager.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Soldier::~Enemy_Soldier() {
	for (auto* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy_Soldier::Initialize() {

	Enemy_InitializeCommon();
	object->SetModelFile("enemy.obj");

	//体力の初期化
	HP_Initialize(3);

	//見える範囲初期化
	eyeReach = { 15, 10, 1 };
}

void Enemy_Soldier::Update() {

	//敵の共有処理
	UpdateCommon();

	if (!isDead) {
		if (!isFoundTarget && !isBullet && !lost_player) {
			SearchRange();
			MoveEnemy();
		}
	}

	particle_fire->SetRotate({ 0,0,-wt.rotation_.y });

	//更新が終了
	UpdateBehind();


#ifdef _DEBUG

	ImGui::Begin("Enemy_soldier");

	ImGui::Text("route_point1 : %f,%f,%f", route_point1.x, route_point1.y, route_point1.z);
	ImGui::Text("route_point2 : %f,%f,%f", route_point2.x, route_point2.y, route_point2.z);

	ImGui::End();

#endif // _DEBUG
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
			isBullet = false;
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
	bullet->SetPlayer(player_);
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}

void Enemy_Soldier::RespawnEnemy() {
	RespawnEnemyCommon();
	rapidCount = 0;
	coolTime = 0;
	isBullet = false;
}