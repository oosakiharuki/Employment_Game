#include "EnemyBullet.h"
#include "Player.h"

using namespace MyMath;

EnemyBullet::~EnemyBullet() {}


void EnemyBullet::Initialize() {
	//弾丸のモデル
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModelFile("PlayerBullet.obj");

	wt.Initialize();

	//弾の当たり判定の大きさ
	bulletAABB.min = { -0.5f,-0.5f,-0.5f };
	bulletAABB.max = { 0.5f,0.5f,0.5f };
}

void EnemyBullet::Update() {
	if (!isPari) {
		//velocity向きに等速直線運動
		wt.translation_ += velocity_;
	}
	else {
		///パリィされた時
		wt.translation_ -= velocity_;
	}


	deathTimer += 1.0f / 60.0f;

	//時間がたったら消える
	if (deathTimer >= endTime) {
		isDead = true;
	}

	object->Update(wt);
	wt.UpdateMatrix();
}

void EnemyBullet::Draw() {
	object->Draw();
}

AABB EnemyBullet::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + bulletAABB.min;
	aabb.max = wt.translation_ + bulletAABB.max;
	return aabb;
}

void EnemyBullet::IsHit() { 
	Vector3 enemyPosition = wt.translation_;
	Vector3 playerPosition = player_->GetWorldPosition();
	//どの位置でぶつかった記録
	distance = enemyPosition - playerPosition;
	//消えるフラグ
	isDead = true;
}

void EnemyBullet::Pari_Mode() {
	//跳ね返るフラグ
	isPari = true;
}
