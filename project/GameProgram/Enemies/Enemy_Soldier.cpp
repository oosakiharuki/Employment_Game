#include "Enemy_Soldier.h"
#include "Input.h"

#include "ImGuiManager.h"
#include "Object3dCommon.h"

using namespace MyMath;
using namespace UseEveryOne;

Enemy_Soldier::~Enemy_Soldier() {
	for (auto& bullet : bullets_) {
		bullet.reset();
	}
}

void Enemy_Soldier::Initialize() {
	//敵の共通初期化処理
	Enemy_InitializeCommon("enemy.obj");

	//体力の初期化
	HP_Initialize(kHp_);

	//見える範囲初期化
	eyeReach_ = kEyeReach_;

	//最大弾丸数
	rapidCountMax_ = kRapidCountMax_;
}

void Enemy_Soldier::UpdateNormal() {
	//
	MoveEnemy();
}

void Enemy_Soldier::UpdateAttack() {
	//見つけたリアクション
	FoundRiaction();

	//コーンが上向きなので
	particles_[particleFire_.name]->SetRotate({ 0.0f,0.0f,-wt_.rotation_.y });
}

void Enemy_Soldier::UpdateDead() {
	//死んだリアクション
	DeadReaction();
}

void Enemy_Soldier::UpdateImgui() {

#ifdef USE_IMGUI

	ImGui::Begin("Enemy_soldier");

	ImGui::Text("translate : %f,%f,%f", wt_.translation_.x, wt_.translation_.y, wt_.translation_.z);
	ImGui::Text("translate : %f,%f,%f", wt_.rotation_.x, wt_.rotation_.y, wt_.rotation_.z);

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
	particles_[particleFire_.name]->SetTranslate(enemyPosition);

	//弾丸を生み出す
	std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>();
	bullet->Initialize();
	bullet->SetPlayer(player_);//プレイヤーと当たりノックバックパラメータで使う
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocty(velocity_);
	bullets_.push_back(std::move(bullet));
}

void Enemy_Soldier::RespawnEnemy() {
	RespawnEnemyCommon();

	//発泡処理のリセット
	rapidCount_ = 0;
	coolTime_ = 0;
	isBullet_ = false;
}