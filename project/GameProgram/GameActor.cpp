#include "GameActor.h"

using namespace UseEveryOne;
using namespace MyMath;

void GameActor::Actor_InitializeCommon() {
	//行列の初期化
	wt_.Initialize();

	//影の初期化
	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();
}

void GameActor::IsGround(bool result) {
	isGround_ = result;
	//地面なら重力を0にする(沈まないようにする)
	if (isGround_) {
		grabity_ = 0;
	}
}

AABB GameActor::GetAABB() {
	AABB aabb;
	aabb.min = wt_.translation_ + actorAABB_.min;
	aabb.max = wt_.translation_ + actorAABB_.max;
	return aabb;
}

void GameActor::ShadowUpdate() {
	shadow_->Update();
}

void GameActor::ScaleUpdate(bool* mosionOn, Vector3 scale, const float& maxTime) {
	if (scaleTimer_ >= maxTime * kDivideByTwo_) {
		wt_.scale_ -= scale;
		if (scaleTimer_ >= maxTime) {
			scaleTimer_ = 0.0f;
			wt_.scale_ = kDefaultScale_;

			//モーションを終了する
			*mosionOn = false;
		}
	}
	else {
		wt_.scale_ += scale;
	}
	scaleTimer_ += kDeltaTime_;
}

void GameActor::RespawnCommon() {
	isDead_ = false;
	hp_ = maxHp_;//体力を満タンに

	//blenderで配置した設定に戻る
	wt_.translation_ = initTranslate_;
	wt_.rotation_ = initRotate_;
}

void GameActor::HP_Initialize(const uint32_t& max) {
	maxHp_ = max;//最大値を設定
	hp_ = maxHp_;
}

Vector3 GameActor::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = wt_.matWorld_.m[3][0];
	worldPos.y = wt_.matWorld_.m[3][1];
	worldPos.z = wt_.matWorld_.m[3][2];

	return worldPos;
}
