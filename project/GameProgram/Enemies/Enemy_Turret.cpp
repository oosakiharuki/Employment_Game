#include "Enemy_Turret.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"

using namespace MyMath;

Enemy_Turret::~Enemy_Turret() {
	for (auto* bullet : bullets_) {
		delete bullet;
	}
}


void Enemy_Turret::Initialize() {

	Enemy_InitializeCommon();

	object->SetModelFile("cannon.obj");

	HP_Initialize(6);

	//見える範囲初期化
	eyeReach = { 15, 2, 1 };
}

void Enemy_Turret::Update() {

	//敵の共有処理
	UpdateCommon();

	if (!isDead) {
		SearchRange();
	}

	//更新が終了
	UpdateBehind();

	particle_fire->SetScale({ 2,2,2 });
	particle_fire->SetRotate({ 0,0,-wt.rotation_.y });


	particle_fire->Update();

#ifdef _DEBUG

	ImGui::Begin("Enemy_Turret");

	ImGui::Text("translate : %f,%f,%f", wt.translation_.x, wt.translation_.y, wt.translation_.z);
	ImGui::Text("translate : %f,%f,%f", wt.rotation_.x, wt.rotation_.y, wt.rotation_.z);

	ImGui::Text("Eye_Min : %f,%f,%f", eyeAABB.min.x, eyeAABB.min.y, eyeAABB.min.z);
	ImGui::Text("Eye_Max : %f,%f,%f", eyeAABB.max.x, eyeAABB.max.y, eyeAABB.max.z);

	ImGui::End();

#endif // _DEBUG

}

void Enemy_Turret::Draw() {

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
	wt.translation_.x - 1.0f,
	wt.translation_.y + 1.0f,
	wt.translation_.z
	};


	particle_fire->SetTranslate(translate);


	Vector3 velocity = { 0.0f,0.0f,0.5f };
	velocity = TransformNormal(velocity, wt.matWorld_);

	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetPlayer(player_);
	bullet->SetTranslate(translate);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}

void Enemy_Turret::RespawnEnemy() {
	RespawnEnemyCommon();
	rapidCount = 0;
	coolTime = 0;
	isBulletStart = false;
}