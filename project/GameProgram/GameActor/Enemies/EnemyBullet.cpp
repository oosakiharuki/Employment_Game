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

	collisionType_ = CollisionTypes::enemyBullet;
}

void EnemyBullet::Update() {
	if (!isParry) {
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

	//当たり判定設定
	collisionAABB_.min = transform_.translate + -kBulletSize_ * kDivideByTwo_;
	collisionAABB_.max = transform_.translate + kBulletSize_ * kDivideByTwo_;

	center_ = transform_.translate;//真ん中の座標
	CollisionManager::GetInstance().AddCollisions(this);
}

void EnemyBullet::Draw() {
	object->Draw();
}

void EnemyBullet::OnCollision(CollisionSource* collision) {
	//消えるフラグ
	isDead_ = true;
}

void EnemyBullet::Parry_Mode() {
	//跳ね返るフラグ
	isParry = true;
}
