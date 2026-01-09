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
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	//弾の当たり判定の大きさ
	bulletAABB.min = -kBulletSize_ * kDivideByTwo_;
	bulletAABB.max = kBulletSize_ * kDivideByTwo_;
}

void EnemyBullet::Update() {
	if (!isPari) {
		//velocity向きに等速直線運動
		transform_.translate += velocity_;
	}
	else {
		///パリィされた時
		transform_.translate -= velocity_;
	}


	deathTimer += kDeltaTime_;

	//時間がたったら消える
	if (deathTimer >= kEndTime) {
		isDead_ = true;
	}

	object->Update(wt_);
	wt_.UpdateMatrix(transform_);
}

void EnemyBullet::Draw() {
	object->Draw();
}

AABB EnemyBullet::GetAABB() const {
	AABB aabb;
	aabb.min = transform_.translate + bulletAABB.min;
	aabb.max = transform_.translate + bulletAABB.max;
	return aabb;
}

void EnemyBullet::IsHit() { 
	Vector3 enemyPosition = transform_.translate;
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
