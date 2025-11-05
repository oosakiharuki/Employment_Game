#include "IEnemy.h"
#include "ImGuiManager.h"

using namespace MyMath;
IEnemy::IEnemy() {}

IEnemy::~IEnemy(){}

void IEnemy::InitializeCommon() {

}

void IEnemy::UpdateCommon() {
	if (hp == 0) {
		isDead = true;
	}

	DeadUpdate();

	if (!isDead && !isPerformance) {

		//重力
		GrabityUpdate();

		DirectionDegree();

		PlayerTarget();

		if (isFoundTarget) {
			isBulletStart = true;
		}

		if (isBulletStart) {
			Attack();
		}
		else {
			rapidCount = 0;
			rapidFireTime = 0;
			coolTime = 0;
		}

		
	}

	shadow_->SetTranslate(wt.translation_);

	if (isDamageMosion) {
		ScaleUpdate(&isDamageMosion, damageScale, damageMaxTime);
	}

	for (auto* bullet : bullets_) {
		bullet->Update();
	}

	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});


	particle_damage->Update();
}

void IEnemy::UpdateBehind() {
	object->Update(wt);
	wt.UpdateMatrix();
}

void IEnemy::IsDamage() {
	particle_damage->SetTranslate(wt.translation_);
	particle_damage->ChangeMode(BornParticle::MomentMode);
	isDamageMosion = true;

	//連続ヒット時、元に戻す
	wt.scale_ = defaultScale;
	scaleTimer = 0.0f;

	if (hp == 0) {
		return;
	}
	hp -= 1;
}


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

void IEnemy::PlayerTarget() {

	if (IsCollisionAABB(player_->GetAABB(), eyeAABB) && !player_->GetIsDead()) {
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

void IEnemy::SearchRange() {
	if (direction == direction_right) {
		eyeAABB.min = wt.translation_ + Vector3(0, -eyeReach.y, -eyeReach.z);
		eyeAABB.max = wt.translation_ + eyeReach;
		speed.x = moveX;
	}
	else if (direction == direction_left) {
		eyeAABB.min = wt.translation_ + -eyeReach;
		eyeAABB.max = wt.translation_ + Vector3(0, eyeReach.y, eyeReach.z);
		speed.x = -moveX;
	}
}

void IEnemy::MoveEnemy() {
	wt.translation_ += speed;
	move += speed;

	//移動ポイントの端だと向きを変える
	if (move.x < route_point1.x) {
		direction = direction_right;
	}
	if (move.x > route_point2.x) {
		direction = direction_left;
	}

	wt.rotation_.y = direction;
}


void IEnemy::RespawnEnemyCommon() {
	RespawnCommon();

	deleteEnemy = false;
	isBulletStart = false;//攻撃はしない

	//向きリセット
	DirectionDegree();
	//移動ルート位置戻す
	move = { 0,0,0 };

	//弾はすべて消す
	bullets_.remove_if([](EnemyBullet* bullet) {
		delete bullet;
		return true;
	});
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
	if (wt.rotation_.y < 0) wt.rotation_.y += 360.0;

	///0~180は右
	if (wt.rotation_.y >= 0.0f && wt.rotation_.y < 180.0f) {
		direction = direction_right;
	}///180~360は左
	else if (wt.rotation_.y <= 360.0f) {
		direction = direction_left;
	}

	wt.rotation_.y = direction;

}
