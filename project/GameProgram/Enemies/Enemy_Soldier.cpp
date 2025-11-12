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
	object->SetModelFile("enemy.obj");

	//体力の初期化
	HP_Initialize(3);

	//見える範囲初期化
	eyeReach = { 15, 10, 1 };

	//最大弾丸数
	rapidCountMax = 3;
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

	//コーンが上向きなので
	particle_fire->SetRotate({ 0,0,-wt.rotation_.y });

	//更新が終了
	UpdateBehind();


#ifdef USE_IMGUI

	ImGui::Begin("Enemy_soldier");

	ImGui::Text("route_point1 : %f,%f,%f", route_point1.x, route_point1.y, route_point1.z);
	ImGui::Text("route_point2 : %f,%f,%f", route_point2.x, route_point2.y, route_point2.z);

	ImGui::End();

#endif // USE_IMGUI
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
	//発泡処理
	Fire();
}

void Enemy_Soldier::FireBullet() {
	
	Vector3 enemyPosition;

	enemyPosition.x = wt.matWorld_.m[3][0];
	enemyPosition.y = wt.matWorld_.m[3][1];
	enemyPosition.z = wt.matWorld_.m[3][2];

	//プレイヤーの方向に向かう(最初に打つ弾にそって進む)
	if (rapidCount == 0) {

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

		velocity = normal;
	}
	particle_fire->SetTranslate(enemyPosition);

	//弾丸を生み出す
	EnemyBullet* bullet = new EnemyBullet();
	bullet->Initialize();
	bullet->SetPlayer(player_);//プレイヤーと当たりノックバックパラメータで使う
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocty(velocity);
	bullets_.push_back(bullet);
}

void Enemy_Soldier::RespawnEnemy() {
	RespawnEnemyCommon();

	//発泡処理のリセット
	rapidCount = 0;
	coolTime = 0;
	isBullet = false;
}