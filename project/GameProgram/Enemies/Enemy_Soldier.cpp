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
	//モデル作成
	object_->SetModelFile("enemy.obj");

	//体力の初期化
	HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	//最大弾丸数
	rapidCountMax_ = kRapidCountMax_;
}

void Enemy_Soldier::Update() {

	//敵の共有処理
	UpdateCommon();

	if (!isDead_) {
		if (!isFoundTarget_ && !isBullet_ && !isLostPlayer_) {
			SearchRange();
			MoveEnemy();
		}
	}

	//コーンが上向きなので
	particleFire_->SetRotate({ 0.0f,0.0f,-wt_.rotation_.y });

	//更新が終了
	UpdateBehind();


#ifdef USE_IMGUI

	ImGui::Begin("Enemy_soldier");

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
	for (auto* bullet : bullets_) {
		bullet->Draw();
	}
}

void Enemy_Soldier::DrawParticle() {
	particleFire_->Draw();
	particleDamage_->Draw();
}

void Enemy_Soldier::Attack() {
	//発泡処理
	Fire();
}

void Enemy_Soldier::FireBullet() {
	
	Vector3 enemyPosition;

	enemyPosition.x = wt_.matWorld_.m[3][0];
	enemyPosition.y = wt_.matWorld_.m[3][1];
	enemyPosition.z = wt_.matWorld_.m[3][2];

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
		normal.x *= kSpeed;
		normal.y *= kSpeed;
		normal.z *= kSpeed;

		velocity_ = normal;
	}
	particleFire_->SetTranslate(enemyPosition);

	//弾丸を生み出す
	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetPlayer(player_);//プレイヤーと当たりノックバックパラメータで使う
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocty(velocity_);
	bullets_.push_back(bullet);
}

void Enemy_Soldier::RespawnEnemy() {
	RespawnEnemyCommon();

	//発泡処理のリセット
	rapidCount_ = 0;
	coolTime_ = 0;
	isBullet_ = false;
}