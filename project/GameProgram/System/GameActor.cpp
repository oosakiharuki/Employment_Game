#include "GameActor.h"

using namespace UseEveryOne;
using namespace MyMath;

void GameActor::Actor_InitializeCommon() {
	//行列の初期化
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	//影の初期化
	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();

	//リアクションクラス
	reaction_ = std::make_unique<Reaction>();
}

void GameActor::IsGround(bool result) {
	isGround_ = result;
	//地面なら重力を0にする(沈まないようにする)
	if (isGround_) {
		gravity_ = 0;
	}
}

AABB GameActor::GetAABB() const {
	AABB aabb;
	aabb.min = transform_.translate + actorAABB_.min;
	aabb.max = transform_.translate + actorAABB_.max;
	return aabb;
}

void GameActor::ShadowUpdate() {
	shadow_->Update();
}

void GameActor::RespawnCommon() {
	isDead_ = false; //死亡フラグをなしに
	hp_ = maxHp_;    //体力を満タンに

	//blenderで配置した設定に戻る
	transform_.translate = initTranslate_; //座標位置
	transform_.rotate = initRotate_;       //回転角度
}

void GameActor::HP_Initialize(uint32_t max) {
	maxHp_ = max;//最大値を設定
	hp_ = maxHp_;//体力を設定
}

Vector3 GameActor::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = wt_.GetMatWorld().m[3][0];
	worldPos.y = wt_.GetMatWorld().m[3][1];
	worldPos.z = wt_.GetMatWorld().m[3][2];

	return worldPos;
}
