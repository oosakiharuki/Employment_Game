#include "Enemy_Turret.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

Enemy_Turret::~Enemy_Turret() {
	for (auto& bullet : bullets_) {
		bullet.reset();
	}
}


void Enemy_Turret::Initialize() {
	//敵の共通初期化処理
	Enemy_InitializeCommon("cannon.obj");

	//体力の初期化
	HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	//最大弾丸数
	rapidCountMax_ = kRapidCountMax_;

	//視野範囲に合わせるためサイズを変更
	particleLaserSize_.x = eyeReach_.x * kDivideByTwo_;
	//パラメータに代入する
	particleLaser_.basicSize = particleLaserSize_;

	//レーザー(見える範囲)の初期化処理
	particles_[particleLaser_.name] = ParticleManager::GetInstance().InitParticle(particleLaser_);
	//攻撃(発泡)
	particles_[particleFire_.name] = ParticleManager::GetInstance().InitParticle(particleFire_);
	//ちょっと大きく
	particles_[particleFire_.name]->SetScale(kParticleFireSize_);
}

void Enemy_Turret::SearchCommand() {
	//レーザーポイント
	LaserPoint();
}

void Enemy_Turret::AttackCommand(){
	if (isFoundTarget_) {
		//見つけたリアクション
		FoundReaction();

		//発砲処理
		Fire();
		if (coolTime_ == 0.0f) {
			isFoundTarget_ = false;
		}
	}

	//レーザーポイント
	LaserPoint();
	//マークの更新
	MarkUpdate();

}

void Enemy_Turret::Active() {

	//敵のステートパターンの更新処理
	StatePatternUpdate();

	PlayerTarget();

	//重力
	//GravityUpdate(transform_.translate.y);
	//弾丸の更新
	BulletUpdate();

	//コーンが上向きなので
	particles_[particleFire_.name]->SetRotate({ 0,0,-transform_.rotate.y });
}

void Enemy_Turret::Dead() {
	//レーザーのパーティクル停止
	particles_[particleLaser_.name]->SetParticleBorn(ParticleBorn::Stop);

	//死んだリアクション
	DeadReaction();
}

void Enemy_Turret::Performance() {}

void Enemy_Turret::UpdateImGui() {

#ifdef USE_IMGUI

	ImGui::Begin("Enemy_Turret");

	ImGui::Text("translate : %f,%f,%f", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("translate : %f,%f,%f", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);

	ImGui::Text("Eye_Min : %f,%f,%f", eyeAABB_.min.x, eyeAABB_.min.y, eyeAABB_.min.z);
	ImGui::Text("Eye_Max : %f,%f,%f", eyeAABB_.max.x, eyeAABB_.max.y, eyeAABB_.max.z);

	ImGui::End();

#endif // USE_IMGUI
}

void Enemy_Turret::Draw() {

	if (!isDeleteEnemy_) {
		object_->Draw();
		shadow_->Draw();//影
	}

	for (auto& bullet : bullets_) {
		bullet->Draw();//弾丸
	}
}

void Enemy_Turret::FireBullet() {
	
	Vector3 translate = transform_.translate;
	//少し前から弾丸が出るように
	//ダメージリアクションで大きくなった時発射位置が変ってしまうためスケールはDefaultに
	translate += TransformNormal(Vector3{ 0,0,kBulletTranslate_ }, MakeAffineMatrix(kDefaultScale_,transform_.rotate,transform_.translate));//タレットの前向きから

	//パーティクルの場所変更
	particlePosition_ = transform_.translate;
	particles_[particleFire_.name]->SetTranslate(particlePosition_);
	
	//飛ばす方向
	Vector3 velocity = { 0.0f,0.0f,kBulletSpeed_ };
	//ダメージリアクションで大きくなった時弾が速くなるためスケールはDefaultに
	velocity = TransformNormal(velocity, MakeAffineMatrix(kDefaultScale_, transform_.rotate, transform_.translate));

	std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>();
	bullet->Initialize();
	bullet->SetPlayer(player_);
	bullet->SetTranslate(translate);
	bullet->SetVelocity(velocity);
	bullets_.push_back(std::move(bullet));

	particles_[particleFire_.name]->SetParticleBorn(ParticleBorn::MomentMode);//パーティクルが出てくる
}

void Enemy_Turret::LaserPoint() {
	//スケール以外の行列
	Matrix4x4 matWorld = MakeAffineMatrix(kDefaultScale_, transform_.rotate, transform_.translate);
	//レーザーサイズXはターレットの前に出すため
	particles_[particleLaser_.name]->SetTranslate(transform_.translate + TransformNormal(Vector3{ 0,0,particleLaserSize_.x }, matWorld));
	particles_[particleLaser_.name]->SetParticleBorn(ParticleBorn::TimerMode);
}

void Enemy_Turret::RespawnEnemy() {
	RespawnEnemyCommon();
	rapidCount_ = 0;
	coolTime_ = 0;
}