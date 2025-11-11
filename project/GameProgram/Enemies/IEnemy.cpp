#include "IEnemy.h"
#include "ImGuiManager.h"

using namespace MyMath;
IEnemy::IEnemy() {}

IEnemy::~IEnemy(){}

void IEnemy::Enemy_InitializeCommon() {
	Actor_InitializeCommon();

	object = std::make_unique<Object3d>();
	object->Initialize();

	object_found = std::make_unique<Object3d>();
	object_found->Initialize();
	object_found->SetModelFile("player_found_mark.obj");

	object_noFound = std::make_unique<Object3d>();
	object_noFound->Initialize();
	object_noFound->SetModelFile("player_lost_mark.obj");


	particle_fire = std::make_unique<Particle>();
	particle_fire->Initialize("enemySoldier_fire", "resource/Sprite/cone.png", PrimitiveType::cone);
	particle_fire->SetParticleCount(1);
	particle_fire->ChangeMode(BornParticle::Stop);
	particle_fire->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.1f);


	particle_damage = std::make_unique<Particle>();
	particle_damage->Initialize("enemySoldier_damage", "resource/Sprite/circle.png", PrimitiveType::ring);
	particle_damage->SetParticleCount(10);
	particle_damage->ChangeMode(BornParticle::Stop);
	particle_damage->SetParticleMosion(ParticleMosion::Exprosion);
	particle_damage->SetFrequency(0.7f);

	wtMark.Initialize();
}

void IEnemy::UpdateCommon() {
	if (hp == 0) {
		isDead = true;
	}

	DeadUpdate();

	if (!isDead && !isPerformance) {

		//重力
		GrabityUpdate();
		//角度
		DirectionDegree();
		//プレイヤーの発見
		PlayerTarget();

		//見つかったら
		if (isFoundTarget) {
			isBullet = true;
			if (lost_player) {
				lost_player = false;
				markTimer = 0.0f;
			}
		}

		markTimer = std::clamp(markTimer, 0.0f, markMaxTime);
		
		if (isBullet) {
			//攻撃
			//攻撃し終わるとisBulletがfalseに
			Attack();
			//!マーク表示時間
			markTimer += deltaTime;	
		}
		else if(!isBullet){
			//攻撃、見つけたマークのタイマーリセット
			rapidCount = 0;
			rapidFireTime = 0;
			coolTime = 0;
			lost_player = true;//見失うフラグ
			markTimer -= deltaTime * 0.5f;
		}		
				
		if(lost_player && markTimer <= 0.0f)
			lost_player = false;
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


}

void IEnemy::UpdateBehind() {
	object->Update(wt);
	wt.UpdateMatrix();

	particle_damage->Update();
	particle_fire->Update();

	if (isDead) return;//死んでるなら読み取らない

	wtMark.translation_ = wt.translation_;
	wtMark.translation_.y += 2.0f;

	wtMark.UpdateMatrix();
	object_found->Update(wtMark);
	object_noFound->Update(wtMark);
}

void IEnemy::DrawCommon() {
	if (isDead) return;//死んでるなら読み取らない

	if(isBullet && markTimer < markMaxTime)
		object_found->Draw();

	if (lost_player && markTimer > 0.0f)
		object_noFound->Draw();
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
	isBullet = false;//攻撃はしない

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

AABB IEnemy::GetBombAABB() { 
	AABB null{};
	return null;
}

Vector3 IEnemy::GetDistance(){
	Vector3 null{};
	return null;
}

bool IEnemy::IsExplosion() { return false; }