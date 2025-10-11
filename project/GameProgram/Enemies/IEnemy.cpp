#include "IEnemy.h"
#include "ImGuiManager.h"

using namespace MyMath;
IEnemy::IEnemy() {}

IEnemy::~IEnemy(){}


void IEnemy::GrabityUpdate() {
	
	grabity -= 0.01f;
	//重力
	if (!isGround) {
		wt.translation_.y += grabity;
	}
	else {
		grabity = 0.0f;
	}

}

AABB IEnemy::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + enemyAABB.min;
	aabb.max = wt.translation_ + enemyAABB.max;
	return aabb;
}



void IEnemy::PlayerTarget() {

	if (IsCollisionAABB(player_->GetAABB(), eyeAABB) && !player_->GetIsPlayerDown()) {
		isFoundTarget = true;

		Segment segment;
		segment.origin = wt.translation_;
		segment.diff = player_->GetTranslate();

		//playerと敵との間に壁があるならば
		for (auto& stage : stages) {
			if (IsCollisionAABB_Segment(stage, segment)) {
				isFoundTarget = false;
				break;
			}
		}
	}
	else {
		isFoundTarget = false;
	}

}

void IEnemy::RespownEnemy_All() {
	isDead = false;
	deleteEnemy = false;
	hp = maxHp;

	//blenderで配置した初期位置に戻る
	wt.translation_ = init_point;
	wt.rotation_ = init_rotate;
}

void IEnemy::DeadUpdate() {
	if (isDead) {
		wt.rotation_.z += 3.0f;
	}
	else {
		wt.rotation_.z = 0.0f;
		return;
	}

	if (wt.rotation_.z > 90.0f) {
		deleteEnemy = true;
	}
}

void IEnemy::DirectionDegree() {

	//0~360にする
	wt.rotation_.y = std::fmod(wt.rotation_.y, 360.0f);
	//-の場合
	if (wt.rotation_.y < 0)
		wt.rotation_.y += 360.0;


	///0~180は右
	if (wt.rotation_.y >= 0.0f && wt.rotation_.y < 180.0f) {
		direction = right;
	}///180~360は右
	else if (wt.rotation_.y <= 360.0f) {
		direction = left;
	}
}

void IEnemy::ScaleUpdate(bool* mosionOn, Vector3 scale,const float maxTime) {
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

void IEnemy::ShadowUpdate() {
	shadow_->Update();
}