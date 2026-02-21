#include "Enemy_Soldier.h"
#include "Input.h"

#include "ImGuiManager.h"
#include "Object3dCommon.h"
#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

Enemy_Soldier::~Enemy_Soldier() {
	for (auto& bullet : bullets_) {
		bullet.reset();
	}
}

void Enemy_Soldier::Initialize() {
	//敵の共通初期化処理
	BaseEnemy::Enemy_InitializeCommon("enemy.obj");

	//体力の初期化
	GameActor::HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	//最大弾丸数
	rapidCountMax_ = kRapidCountMax_;

	//攻撃(発泡)
	particles_[particleFire_.name] = ParticleManager::GetInstance().InitParticle(particleFire_);
}

void Enemy_Soldier::Move() {
	move_ += speed_;//移動ポイント
	
	//方向転換
	//敵が右向き
	if (transform_.rotate.y == kDirectionRight_) {
		speed_.x = kMoveX_;//右に進む
		eyeReach_.x = kEyeReach_.x;
	}
	//左向き
	else if (transform_.rotate.y == kDirectionLeft_) {
		speed_.x = -kMoveX_;//左に進む
		eyeReach_.x = -kEyeReach_.x;
	}
}


void Enemy_Soldier::Active() {
	//敵のステートパターンの更新処理
	StatePatternUpdate();

	//重力
	GravityUpdate(transform_.translate.y);
	PlayerTarget();
	//弾丸の更新
	BulletUpdate();

	isGround_ = false;
}

void Enemy_Soldier::SearchCommand() {
	//移動する
	Move();
	isFire_ = true;
}

void Enemy_Soldier::AttackCommand() {
	
	if (isFire_) {
		//見つけたリアクション
		FoundReaction();

		//発砲処理
		Fire();
	}

	if (!isFire_ && !enemyEye_->IsFound()) {
		attackSwitch_ = false;
	}
	else {
		isFire_ = true;
	}

	//マークの更新
	MarkUpdate();

	//コーンが上向きなので
	particles_[particleFire_.name]->SetRotate({ 0.0f,0.0f,-transform_.rotate.y });
}

void Enemy_Soldier::Dead() {
	//死んだリアクション
	DeadReaction();
}

void Enemy_Soldier::Performance() {}

void Enemy_Soldier::UpdateImGui() {

#ifdef USE_IMGUI

	ImGui::Begin("Enemy_soldier");

	ImGui::Text("translate : %f,%f,%f", transform_.translate.x, transform_.translate.y, transform_.translate.z);
	ImGui::Text("translate : %f,%f,%f", transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);

	ImGui::Text("routePointLeft : %f,%f,%f", routePointLeft_.x, routePointLeft_.y, routePointLeft_.z);
	ImGui::Text("routePointRight : %f,%f,%f", routePointRight_.x, routePointRight_.y, routePointRight_.z);

	ImGui::End();

#endif // USE_IMGUI
}

void Enemy_Soldier::Draw() {
	if (!isDeleteEnemy_) {
		object_->Draw();
		shadow_->Draw();
	}
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void Enemy_Soldier::FireBullet() {
	
	Vector3 enemyPosition;
	enemyPosition = { wt_.GetMatWorld().m[3][0],wt_.GetMatWorld().m[3][1],wt_.GetMatWorld().m[3][2]};

	//プレイヤーの方向に向かう(最初に打つ弾にそって進む)
	if (rapidCount_ == 0) {

		const float kSpeed = 0.4f;
		//プレイヤーの座標
		Vector3 playerPosition = player_->GetWorldPosition();
		//敵とプレイヤーの距離
		Vector3 distance = playerPosition - enemyPosition;
		//
		Vector3 normal = Normalize(distance);

		//スピードを合わせる
		normal *= kSpeed;
		velocity_ = normal;
	}
	particles_[particleFire_.name]->SetTranslate(enemyPosition);

	//弾丸を生み出す
	std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>();
	bullet->Initialize();
	bullet->SetPlayer(player_);//プレイヤーと当たりノックバックパラメータで使う
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocity(velocity_);
	bullets_.push_back(std::move(bullet));

	particles_[particleFire_.name]->SetParticleBorn(ParticleBorn::MomentMode);//パーティクルが出てくる
}

void Enemy_Soldier::RespawnEnemy() {
	RespawnEnemyCommon();

	//発泡処理のリセット
	rapidCount_ = 0;
	coolTime_ = 0;
}


void Enemy_Soldier::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::playerBullet ||
		collision->GetType() == CollisionTypes::parryBullet) {
		IsDamage();
	}

	if (collision->GetType() == CollisionTypes::stage) {
		CollisionManager::GetInstance().GameActorAndStageCollision(collisionOverlap, *this, *this, collision->GetAABB());
	}
}
