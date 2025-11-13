#include "GameActor.h"

using namespace MyMath;

void GameActor::Actor_InitializeCommon() {
	//行列の初期化
	wt.Initialize();

	//影の初期化
	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();
}

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
	scaleTimer += kDeltaTime;
}

void GameActor::RespawnCommon() {
	isDead = false;
	hp = maxHp;//体力を満タンに

	//blenderで配置した設定に戻る
	wt.translation_ = init_point;
	wt.rotation_ = init_rotate;
}

void GameActor::HP_Initialize(const uint32_t& max) {
	maxHp = max;//最大値を設定
	hp = maxHp;
}

Vector3 GameActor::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = wt.matWorld_.m[3][0];
	worldPos.y = wt.matWorld_.m[3][1];
	worldPos.z = wt.matWorld_.m[3][2];

	return worldPos;
}
