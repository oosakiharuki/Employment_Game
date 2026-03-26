#include "Enemy_Turret.h"
#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

Enemy_Turret::~Enemy_Turret() {
	fireCommand_->BulletReset();
}

void Enemy_Turret::Initialize() {
	//敵の共通初期化処理
	Enemy_InitializeCommon("cannon.obj");

	//体力の初期化
	HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	//
	fireCommand_ = std::make_unique<EnemyFireCommand>();
	//最大弾丸数
	fireCommand_->SetRapidCountMax(kRapidCountMax_);

	////視野範囲に合わせるためサイズを変更
	//particleLaserSize_.x = eyeReach_.x * kDivideByTwo_;
	////パラメータに代入する
	//particleLaser_.basicSize = particleLaserSize_;

	//レーザー(見える範囲)の初期化処理
	particles_[particleLaser_] = ParticleManager::GetInstance().InitParticle(particleLaser_);
	particleLaserSize_ = particles_[particleLaser_]->GetScale();
	particleLaserSize_.x = eyeReach_.x * kDivideByTwo_;

	particles_[particleLaser_]->SetScale(particleLaserSize_);
	//攻撃(発泡)
	particles_[fireCommand_->GetParticleFireName()] = ParticleManager::GetInstance().InitParticle(fireCommand_->GetParticleFireName());
	//ちょっと大きく
	particles_[fireCommand_->GetParticleFireName()]->SetScale(kParticleFireSize_);
}

void Enemy_Turret::SearchCommand() {
	//レーザーポイント
	LaserPoint();
	//オンにしておく
	fireCommand_->FireStart();
}

void Enemy_Turret::AttackCommand(){
	if (fireCommand_->IsFire()) {
		//見つけたリアクション
		FoundReaction();

		//発砲処理
		fireCommand_->Fire(*this);
	}
	
	if (!fireCommand_->IsFire() && !enemyEye_->IsFound()) {
		attackSwitch_ = false;
	}
	else {
		fireCommand_->FireStart();
	}

	//レーザーポイント
	LaserPoint();
	//マークの更新
	MarkUpdate();

}

void Enemy_Turret::Active() {

	//敵のステートパターンの更新処理
	StatePatternUpdate();

	if (transform_.rotate.y == kDirectionRight_) {
		eyeReach_.x = kEyeReach_.x;
	}
	//左向き
	else if (transform_.rotate.y == kDirectionLeft_) {
		eyeReach_.x = -kEyeReach_.x;
	}

	PlayerTarget();
	//捜索範囲更新
	SearchRange();

	//重力
	GravityUpdate(transform_.translate.y);
	//弾丸の更新
	fireCommand_->BulletUpdate();

	//コーンが上向きなので
	particles_[fireCommand_->GetParticleFireName()]->SetRotate({0,0,-transform_.rotate.y});

	isGround_ = false;
}

void Enemy_Turret::Dead() {
	//レーザーのパーティクル停止
	particles_[particleLaser_]->SetParticleBorn(ParticleBorn::Stop);

	//死んだリアクション
	DeadReaction();
	//弾丸の更新
	fireCommand_->BulletUpdate();
}

void Enemy_Turret::Performance() {}

void Enemy_Turret::UpdateImGui() {

#ifdef USE_IMGUI

	ImGui::Begin("Enemy_Turret");

	ImGui::Text("translate : %f,%f,%f", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("translate : %f,%f,%f", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);

	//ImGui::Text("Eye_Min : %f,%f,%f", eyeCenter_ - eyeReach_, eyeAABB_.min.y, eyeAABB_.min.z);
	//ImGui::Text("Eye_Max : %f,%f,%f", eyeAABB_.max.x, eyeAABB_.max.y, eyeAABB_.max.z);

	ImGui::End();

#endif // USE_IMGUI
}

void Enemy_Turret::Draw() {

	if (!isDeleteEnemy_) {
		object_->Draw();
		shadow_->Draw();//影
	}
		
	fireCommand_->BulletDraw();//弾丸
}

void Enemy_Turret::FireBullet() {
	
	Vector3 translate = transform_.translate;
	//少し前から弾丸が出るように
	//ダメージリアクションで大きくなった時発射位置が変ってしまうためスケールはDefaultに
	translate += TransformNormal(Vector3{ 0,0,kBulletTranslate_ }, MakeAffineMatrix(kDefaultScale_,transform_.rotate,transform_.translate));//タレットの前向きから

	//パーティクルの場所変更
	particlePosition_ = transform_.translate;
	particles_[fireCommand_->GetParticleFireName()]->SetTranslate(particlePosition_);
	
	//飛ばす方向
	Vector3 velocity = { 0.0f,0.0f,kBulletSpeed_ };
	//ダメージリアクションで大きくなった時弾が速くなるためスケールはDefaultに
	velocity = TransformNormal(velocity, MakeAffineMatrix(kDefaultScale_, transform_.rotate, transform_.translate));

	fireCommand_->AddBullet(particlePosition_, velocity);
	particles_[fireCommand_->GetParticleFireName()]->SetParticleBorn(ParticleBorn::MomentMode);//パーティクルが出てくる
}

void Enemy_Turret::LaserPoint() {
	//スケール以外の行列
	Matrix4x4 matWorld = MakeAffineMatrix(kDefaultScale_, transform_.rotate, transform_.translate);
	//レーザーサイズXはターレットの前に出すため
	particles_[particleLaser_]->SetTranslate(transform_.translate + TransformNormal(Vector3{ 0,0,particleLaserSize_.x }, matWorld));
	particles_[particleLaser_]->SetParticleBorn(ParticleBorn::TimerMode);
}

void Enemy_Turret::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypePlayerBullet) {
		IsDamage();
	}

	if (collision->GetType() == CollisionTypes::TypeStage) {
		CollisionUtility::GetInstance().GameActorAndStageCollision(collisionOverlap, *this, *this, collision->GetAABB());
	}
}

bool Enemy_Turret::TypeCheckUp(const CollisionTypes& collisionType) {
	if (collisionType == CollisionTypes::TypePlayerBullet ||
		collisionType == CollisionTypes::TypeStage) {
		return true;
	}
	return false;
}
