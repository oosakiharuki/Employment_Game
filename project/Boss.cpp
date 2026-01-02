#include "Boss.h"

using namespace MyMath;
using namespace UseEveryOne;

void Boss::Initialize() {
	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile("Boss.gltf");

	wt_.Initialize();

	bossState_ = std::make_unique<BossMoveState>();
}

void Boss::Update() {

	wt_.translation_ += GoDestination(move_) / bunkatu_;

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
	//右位置
	SetMovePoint(kCenter_ + kHazi_, 180.0f);
}

void Boss::MoveLeft() {
	//左位置
	SetMovePoint(kCenter_ - kHazi_, 180.0f);
}

void Boss::SetMovePoint(const Vector3& point, float speedBunkatu) {
	move_.diff = point;//目的地設定
	bunkatu_ = speedBunkatu;//スピード分割
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
	//現在位置の設定
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

void Boss::ArrivedSegmentDiff() {
	Vector3 a = wt_.translation_;

	//目的地についたとき
	if (GoDestination(a, move_.diff) <= Vector3{ 0.1f,0.1f,0.1f } &&
		GoDestination(a, move_.diff) >= Vector3{ -0.1f,-0.1f,-0.1f }) {

		isMoveSucces_ = true;
		wt_.translation_ = move_.diff;//現在地を目的地にする
		move_.origin = wt_.translation_;//セグメントのスタート値を設定
	}
}

void Boss::BeforeActionMosion() {
	wt_.rotation_.z += kRotationX_;

	if (wt_.rotation_.z >= 360.0f) {
		//モーション終了
		isMosionFinish_ = true;
		wt_.rotation_.z = 0.0f;
	}
}
