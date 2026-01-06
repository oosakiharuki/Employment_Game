#include "Boss.h"
#include "ImGuiManager.h"

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

	wt_.translation_ += GoDestination(move_) / moveFrame_;

	if (hp_ == 0 && deadTimer_ < kDeltaTime_) {
		bossState_.reset();
		bossState_ = std::make_unique<BossDeadMosionState>();
	}

	//ステートパターン
	bossState_->Update(*this);

	if (isDamageReaction_) {
		reaction_->ScaleReaction(wt_.scale_, isDamageReaction_,damageReactionPower_,damageReactionTimer_,kDamageReactionTimeMax_);
	}

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


	ImGuiUpdate();

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

void Boss::SetMovePoint(const Vector3& point, float speedBunkatu) {
	move_.diff = point;//目的地設定
	moveFrame_ = speedBunkatu;//スピード分割
}

void Boss::Fire(float kFrame) {

	//連射で時間を開ける
	rapidFireTime_ += kDeltaTime_ / kFrame;
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

	//弾丸速度
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

void Boss::ChangeStatePattern(std::unique_ptr<BaseBossState> state) {
	bossState_.reset();
	bossState_ = std::move(state);
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

AABB Boss::GetAABB() {
	AABB aabb{};
	aabb.max = wt_.translation_ + aabb_.max;
	aabb.min = wt_.translation_ + aabb_.min;
	return aabb;
}

void Boss::IsDamage() {
	if (hp_ == 0) {
		return;
	}
	hp_--;
	isDamageReaction_ = true;
}

void Boss::DeadMosion() {

	isDeadMosion_ = true;

	std::random_device seed;
	std::mt19937 random(seed());

	std::uniform_real_distribution<float> yure(-kShakePower, kShakePower);

	//上下左右にシェイク(z軸は関係ない)
	wt_.translation_ = deadPosition_ + Vector3(yure(random), yure(random), 0.0f);

	wt_.translation_.x = std::clamp(wt_.translation_.x, deadPosition_.x - kShakePower, deadPosition_.x + kShakePower);
	wt_.translation_.y = std::clamp(wt_.translation_.y, deadPosition_.y - kShakePower, deadPosition_.y + kShakePower);

	deadTimer_ += kDeltaTime_;

	wt_.scale_ = deadScale_ - deadTimer_ / kDeadTimeMax_;

	if (deadTimer_ >= kDeadTimeMax_) {
		isDead_ = true;
		wt_.scale_ = { 0,0,0 };//消えるようにする
	}
}

void Boss::DeadPosition() {
	deadPosition_ = wt_.translation_;
	deadScale_ = wt_.scale_;
}

void Boss::ImGuiUpdate() {
#ifdef USE_IMGUI

	ImGui::Begin("boss");
	ImGui::Text("translate: %f, %f, %f", wt_.translation_.x, wt_.translation_.y, wt_.translation_.z);
	ImGui::Text("Hp: %d",hp_);
	ImGui::End();

#endif // USE_IMGUI

}
