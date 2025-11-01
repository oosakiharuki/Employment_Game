#include "IEnemy.h"
#include "ImGuiManager.h"

using namespace MyMath;
IEnemy::IEnemy() {}

IEnemy::~IEnemy(){}

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

void IEnemy::RespownEnemyCommon() {
	isDead = false;
	deleteEnemy = false;
	isBulletStart = false;
	hp = maxHp;

	//blenderで配置した初期位置に戻る
	wt.translation_ = init_point;
	wt.rotation_ = init_rotate;

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