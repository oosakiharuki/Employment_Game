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

AABB GameActor::GetAABB() const {
	AABB aabb;
	aabb.min = wt_.translation_ + actorAABB_.min;
	aabb.max = wt_.translation_ + actorAABB_.max;
	return aabb;
}

void GameActor::ShadowUpdate() {
	shadow_->Update();
}

void GameActor::ScaleUpdate(bool& mosionOn, const Vector3& scale, float maxTime) {
	//時間が半分になったら
	if (scaleTimer_ >= maxTime * kDivideByTwo_) {
		//スケールを小さくする
		wt_.scale_ -= scale;
	}
	else {
		//スケールを大きくする
		wt_.scale_ += scale;
	}

	//経過時間がたったら終了
	if (scaleTimer_ >= maxTime) {
		//時間を初期値(0)にする
		scaleTimer_ = 0.0f;
		//元の大きさに{1,1,1}
		wt_.scale_ = kDefaultScale_;
		//モーションを終了する
		mosionOn = false;
	}	

	//時間がが進む
	scaleTimer_ += kDeltaTime_;		
}

void GameActor::RespawnCommon() {
	isDead_ = false; //死亡フラグをなしに
	hp_ = maxHp_;    //体力を満タンに

	//blenderで配置した設定に戻る
	wt_.translation_ = initTranslate_; //座標位置
	wt_.rotation_ = initRotate_;       //回転角度
}

void GameActor::HP_Initialize(uint32_t max) {
	maxHp_ = max;//最大値を設定
	hp_ = maxHp_;//体力を設定
}

Vector3 GameActor::GetWorldPosition() const {
	Vector3 worldPos;

	worldPos.x = wt_.matWorld_.m[3][0];
	worldPos.y = wt_.matWorld_.m[3][1];
	worldPos.z = wt_.matWorld_.m[3][2];

	return worldPos;
}
