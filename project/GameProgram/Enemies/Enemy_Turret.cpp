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
	//モデル作成
	object_->SetModelFile("cannon.obj");

	HP_Initialize(6);

	//見える範囲初期化
	eyeReach_ = { 20, 0, 0 };

	//レーザー(見える範囲)の初期化処理
	particleLaser_ = std::make_unique<Particle>();
	particleLaser_->Initialize("tullet_laser","resource/Sprite/3YvXH.png",PrimitiveType::beam);
	particleLaser_->SetFrequency(0.001f);
	particleLaser_->SetParticleCount(1);
	particleLaser_->SetParticleBorn(ParticleBorn::TimerMode);
	particleLaser_->SetParticleMosion(ParticleMosion::Fixed);
	particleLaser_->SetScale({ eyeReach_.x * 0.5f,0.1f,0.1f });

	//ちょっと大きく
	particleFire_->SetScale({ 1.5f,1.5f,1.5f });

	//最大弾丸数
	rapidCountMax_ = 6;
}

void Enemy_Turret::Update() {

	//敵の共有処理
	UpdateCommon();

	if (!isDead_) {
		SearchRange();
		Matrix4x4 a = MakeAffineMatrix(Vector3(1,1,1), wt_.rotation_, wt_.translation_);
		particleLaser_->SetTranslate(wt_.translation_ + TransformNormal(Vector3{0,0,eyeReach_.x * 0.5f}, a));
		particleLaser_->SetParticleBorn(ParticleBorn::TimerMode);
	}
	else {
		particleLaser_->SetParticleBorn(ParticleBorn::Stop);
	}

	//コーンが上向きなので
	particleFire_->SetRotate({ 0,0,-wt_.rotation_.y });
	
	//レーザー更新処理
	particleLaser_->Update();

	//更新が終了
	UpdateBehind();

#ifdef USE_IMGUI

	ImGui::Begin("Enemy_Turret");

	ImGui::Text("translate : %f,%f,%f", wt_.translation_.x, wt_.translation_.y, wt_.translation_.z);
	ImGui::Text("translate : %f,%f,%f", wt_.rotation_.x, wt_.rotation_.y, wt_.rotation_.z);

	ImGui::Text("Eye_Min : %f,%f,%f", eyeAABB_.min.x, eyeAABB_.min.y, eyeAABB_.min.z);
	ImGui::Text("Eye_Max : %f,%f,%f", eyeAABB_.max.x, eyeAABB_.max.y, eyeAABB_.max.z);

	ImGui::End();

#endif // USE_IMGUI

}

void Enemy_Turret::Draw() {

	if (!isDeleteEnemy_) {
		object_->Draw();
		shadow_->Draw();
	}

	for (auto* bullet : bullets_) {
		bullet->Draw();
	}

	ParticleCommon::GetInstance()->Command();

	particleFire_->Draw();
	particleDamage_->Draw();
	particleLaser_->Draw();

	Object3dCommon::GetInstance()->Command();

}

void Enemy_Turret::Attack() {
	//発泡処理
	Fire();
}

void Enemy_Turret::FireBullet() {
	
	Vector3 translate = wt_.translation_;
	//少し前から弾丸が出るように
	translate.x -= 2.0f,

	//パーティクルの場所変更
	particlePosition_ = wt_.translation_;
	particleFire_->SetTranslate(particlePosition_);


	Vector3 velocity = { 0.0f,0.0f,0.5f };
	velocity = TransformNormal(velocity, wt_.matWorld_);

	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetPlayer(player_);
	bullet->SetTranslate(translate);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}

void Enemy_Turret::RespawnEnemy() {
	RespawnEnemyCommon();
	rapidCount_ = 0;
	coolTime_ = 0;
	isBullet_ = false;
}