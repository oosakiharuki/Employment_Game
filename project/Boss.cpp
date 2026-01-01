#include "Boss.h"

using namespace MyMath;
using namespace UseEveryOne;

void Boss::Initialize() {
	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("Boss.gltf");

	wt_.Initialize();

	move_.origin = wt_.translation_;
	move_.diff = wt_.translation_;

	kCenter_ = wt_.translation_;

	bossState_ = std::make_unique<BossMoveState>();
}

void Boss::Update() {

	//ステートパターン
	bossState_->Update(*this);

	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	bullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			bullet.reset();
			return true;
		}
		return false;
		});


	object_->Update(wt_);
	wt_.UpdateMatrix();
}

void Boss::Draw() {
	object_->Draw();

	Object3dCommon::GetInstance()->Command();
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void Boss::MoveRight() {
	move_.diff = kCenter_ + kHazi_;
	wt_.translation_.x += Length(move_.origin.x, move_.diff.x) / kBunkatu;
	ArrivedSegmentDiff(wt_.translation_.x >= move_.diff.x);
}

void Boss::MoveLeft() {
	move_.diff = kCenter_ - kHazi_;
	wt_.translation_.x -= Length(move_.origin.x, move_.diff.x) / kBunkatu;
	ArrivedSegmentDiff(wt_.translation_.x <= move_.diff.x);
}

void Boss::Fire() {

	//連射で時間を開ける
	rapidFireTime_ += kDeltaTime_;
	if (rapidFireTime_ >= kRapidFireTimeMax_) {
		FireBullet();//敵の発泡攻撃
		rapidCount_++;//カウント
		rapidFireTime_ = 0;//もう一度
	}

	//最大弾丸数を超えた場合
	if (rapidCount_ == rapidCountMax_) {
		rapidCount_ = 0;//カウントリセット
		isStopFire_ = true;//発砲終了
	}
}

void Boss::FireBullet() {

	Vector3 enemyPosition;
	enemyPosition = { wt_.matWorld_.m[3][0],wt_.matWorld_.m[3][1],wt_.matWorld_.m[3][2] };

	Vector3 velocity;

	//プレイヤーの方向に向かう(最初に打つ弾にそって進む)

	const float kSpeed = 0.4f;
	//プレイヤーの座標
	Vector3 playerPosition = player_->GetWorldPosition();
	//敵とプレイヤーの距離
	Vector3 distance = playerPosition - enemyPosition;
	//
	Vector3 normal = Normalize(distance);

	//スピードを合わせる
	normal *= kSpeed;
	velocity = normal;


	//弾丸を生み出す
	std::unique_ptr<EnemyBullet> bullet = std::make_unique<EnemyBullet>();
	bullet->Initialize();
	bullet->SetPlayer(player_);//プレイヤーと当たりノックバックパラメータで使う
	bullet->SetTranslate(enemyPosition);
	bullet->SetVelocty(velocity);
	bullets_.push_back(std::move(bullet));
}

void Boss::ChangeStatePattern(std::unique_ptr<BaseBossState> playerState) {
	bossState_.reset();
	bossState_ = std::move(playerState);
}

void Boss::ArrivedSegmentDiff(bool isArrived) {
	if (isArrived) {
		isMoveSucces_ = true;
		move_.origin = wt_.translation_;
	}
}

