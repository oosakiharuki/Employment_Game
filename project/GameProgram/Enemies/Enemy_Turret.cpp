#include "Enemy_Turret.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"

using namespace MyMath;
using namespace UseEveryOne;

Enemy_Turret::~Enemy_Turret() {
	for (auto* bullet : bullets_) {
		delete bullet;
	}
}


void Enemy_Turret::Initialize() {

	Enemy_InitializeCommon();
	//モデル作成
	object_->SetModelFile("cannon.obj");

	HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	//最大弾丸数
	rapidCountMax_ = kRapidCountMax_;

	particleLaserSize_.x = eyeReach_.x * kDivideByTwo_;

	//レーザー(見える範囲)の初期化処理
	particleLaser_ = std::make_unique<Particle>();
	particleLaser_->Initialize("tullet_laser","resource/Sprite/3YvXH.png",PrimitiveType::beam);
	particleLaser_->SetParticleCount(kParticleLaserCount_);
	particleLaser_->SetFrequency(kParticleLaserFrequency_);
	particleLaser_->SetParticleBorn(ParticleBorn::TimerMode);
	particleLaser_->SetScale(particleLaserSize_);

	//ちょっと大きく
	particleFire_->SetScale(kParticleFireSize_);
}

void Enemy_Turret::Update() {

	//敵の共有処理
	UpdateCommon();

	if (!isDead_) {
		SearchRange();
		//スケール以外の行列
		Matrix4x4 matWorld = MakeAffineMatrix(kDefaultScale_, wt_.rotation_, wt_.translation_);
		//レーザーサイズXはターレットの前に出すため
		particleLaser_->SetTranslate(wt_.translation_ + TransformNormal(Vector3{0,0,particleLaserSize_.x}, matWorld));
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
}

void Enemy_Turret::DrawParticle() {
	particleFire_->Draw();
	particleDamage_->Draw();
	particleLaser_->Draw();
}

void Enemy_Turret::Attack() {
	//発泡処理
	Fire();
}

void Enemy_Turret::FireBullet() {
	
	Vector3 translate = wt_.translation_;
	//少し前から弾丸が出るように
	translate.x -= kBulletTranslate_,

	//パーティクルの場所変更
	particlePosition_ = wt_.translation_;
	particleFire_->SetTranslate(particlePosition_);
	
	//飛ばす方向
	Vector3 velocity = { 0.0f,0.0f,kBulletSpeed_ };
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