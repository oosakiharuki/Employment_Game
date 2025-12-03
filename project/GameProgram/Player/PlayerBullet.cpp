#include "PlayerBullet.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

PlayerBullet::~PlayerBullet() {}


void PlayerBullet::Initialize() {
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("PlayerBullet.obj");

	wt_.Initialize();

	//AABBの大きさ設定
	bulletAABB_.min = -kBulletSize_ * kDivideByTwo_;
	bulletAABB_.max = kBulletSize_ * kDivideByTwo_;;
}

void PlayerBullet::Update() {

	deathTimer_ += kDeltaTime_;
	
	//弾丸速度
	//徐々に減速する
	wt_.translation_ += EaseOut(velocity_,deathTimer_,kEndTime_);

	//時間がたったら消える
	if (deathTimer_ >= kEndTime_) {
		isDead_ = true;
	}

	object_->Update(wt_);
	wt_.UpdateMatrix();
}

void PlayerBullet::Draw() {
	object_->Draw();
}

AABB PlayerBullet::GetAABB() const {
	AABB aabb;
	aabb.min = wt_.translation_ + bulletAABB_.min;
	aabb.max = wt_.translation_ + bulletAABB_.max;
	return aabb;
}
