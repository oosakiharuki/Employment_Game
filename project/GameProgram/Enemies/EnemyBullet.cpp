#include "EnemyBullet.h"
#include "Player.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

EnemyBullet::~EnemyBullet() {}


void EnemyBullet::Initialize() {
	//弾丸のモデル
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModelFile("PlayerBullet.obj");

	wt_.Initialize();

	//弾の当たり判定の大きさ
	bulletAABB.min = -kBulletSize_ * kDivideByTwo_;
	bulletAABB.max = kBulletSize_ * kDivideByTwo_;
}

void EnemyBullet::Update() {
	if (!isPari) {
		//velocity向きに等速直線運動
		wt_.translation_ += velocity_;
	}
	else {
		///パリィされた時
		wt_.translation_ -= velocity_;
	}


	deathTimer += kDeltaTime_;

	//時間がたったら消える
	if (deathTimer >= kEndTime) {
		isDead_ = true;
	}

	object->Update(wt_);
	wt_.UpdateMatrix();
}

void EnemyBullet::Draw() {
	object->Draw();
}

AABB EnemyBullet::GetAABB() const {
	AABB aabb;
	aabb.min = wt_.translation_ + bulletAABB.min;
	aabb.max = wt_.translation_ + bulletAABB.max;
	return aabb;
}

void EnemyBullet::IsHit() { 
	Vector3 enemyPosition = wt_.translation_;
	Vector3 playerPosition = player_->GetWorldPosition();
	//どの位置でぶつかった記録
	distance = enemyPosition - playerPosition;
	//消えるフラグ
	isDead_ = true;
}

void EnemyBullet::Pari_Mode() {
	//跳ね返るフラグ
	isPari = true;
}
