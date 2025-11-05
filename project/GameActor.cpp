#include "GameActor.h"

using namespace MyMath;

void GameActor::IsGround(bool result) {
	isGround = result;
	//地面なら重力を0にする(沈まないようにする)
	if (isGround) {
		grabity = 0;
	}
}


AABB GameActor::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + actorAABB.min;
	aabb.max = wt.translation_ + actorAABB.max;
	return aabb;
}

void GameActor::ShadowUpdate() {
	shadow_->Update();
}

void GameActor::ScaleUpdate(bool* mosionOn, Vector3 scale, const float& maxTime) {
	if (scaleTimer >= maxTime / 2.0f) {
		wt.scale_ -= scale;
		if (scaleTimer >= maxTime) {
			scaleTimer = 0.0f;
			wt.scale_ = { 1,1,1 };

			//モーションを終了する
			*mosionOn = false;
		}
	}
	else {
		wt.scale_ += scale;
	}
	scaleTimer += deltaTime;
}

void GameActor::RespawnCommon() {
	isDead = false;
	hp = maxHp;//体力を満タンに

	//blenderで配置した設定に戻る
	wt.translation_ = init_point;
	wt.rotation_ = init_rotate;
}

void GameActor::HP_Initialize(const uint32_t& max) {
	maxHp = max;
	hp = maxHp;
}
