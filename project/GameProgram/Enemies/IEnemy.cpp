#include "IEnemy.h"
#include "ImGuiManager.h"

using namespace MyMath;
using namespace UseEveryOne;
IEnemy::IEnemy() {}

IEnemy::~IEnemy(){}

void IEnemy::Enemy_InitializeCommon() {
	Actor_InitializeCommon();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();

	objectFound_ = std::make_unique<Object3d>();
	objectFound_->Initialize();
	objectFound_->SetModelFile("player_found_mark.obj");

	objectNoFound_ = std::make_unique<Object3d>();
	objectNoFound_->Initialize();
	objectNoFound_->SetModelFile("player_lost_mark.obj");


	particleFire_ = std::make_unique<Particle>();
	particleFire_->Initialize("enemy_fire", "resource/Sprite/cone.png", PrimitiveType::cone);
	particleFire_->SetParticleCount(1);
	particleFire_->SetFrequency(0.1f);


	particleDamage_ = std::make_unique<Particle>();
	particleDamage_->Initialize("enemy_damage", "resource/Sprite/circle.png", PrimitiveType::ring);
	particleDamage_->SetParticleCount(20);
	particleDamage_->SetFrequency(0.6f);

	wtMark_.Initialize();
}

void IEnemy::UpdateCommon() {
	if (hp_ == 0) {
		isDead_ = true;
	}

	DeadUpdate();

	if (!isDead_ && !isPerformance_) {

		//重力
		GrabityUpdate();
		//角度
		DirectionDegree();
		//プレイヤーの発見
		PlayerTarget();

		//見つかったら
		if (isFoundTarget_) {
			isBullet_ = true;
			if (isLostPlayer_) {
				isLostPlayer_ = false;
				markTimer_ = 0.0f;
			}
		}

		markTimer_ = std::clamp(markTimer_, 0.0f, kMarkMaxTime_);
		
		if (isBullet_) {
			//攻撃
			//攻撃し終わるとisBulletがfalseに
			Attack();
			//!マーク表示時間
			markTimer_ += kDeltaTime;
		}
		else if(!isBullet_){
			//攻撃、見つけたマークのタイマーリセット
			rapidCount_ = 0;
			rapidFireTime_ = 0;
			coolTime_ = 0;
			isLostPlayer_ = true;//見失うフラグ
			markTimer_ -= kDeltaTime;
		}		
				
		if (isLostPlayer_ && markTimer_ <= 0.0f)
			isLostPlayer_ = false;
	}

	shadow_->SetTranslate(wt_.translation_);

	if (isDamageMosion_) {
		ScaleUpdate(&isDamageMosion_, damageScale_, kDamageMaxTime_);
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
	object_->Update(wt_);
	wt_.UpdateMatrix();

	particleDamage_->Update();
	particleFire_->Update();

	if (isDead_) return;//死んでるなら読み取らない

	wtMark_.translation_ = wt_.translation_;
	wtMark_.translation_.y += kMarkPositionY_;

	wtMark_.UpdateMatrix();
	objectFound_->Update(wtMark_);
	objectNoFound_->Update(wtMark_);
}

void IEnemy::DrawCommon() {
	if (isDead_) return;//死んでるなら読み取らない

	if(isBullet_ && markTimer_ < kMarkMaxTime_)
		objectFound_->Draw();

	if (isLostPlayer_ && markTimer_ > 0.0f)
		objectNoFound_->Draw();
}

void IEnemy::DrawParticle() {}

void IEnemy::IsDamage() {
	particleDamage_->SetTranslate(wt_.translation_);
	particleDamage_->SetParticleBorn(ParticleBorn::MomentMode);
	isDamageMosion_ = true;

	//連続ヒット時、元に戻す
	wt_.scale_ = kDefaultScale_;
	scaleTimer_ = 0.0f;

	if (hp_ == 0) {
		return;
	}
	//体力 -1
	hp_--;
}


void IEnemy::GrabityUpdate() {
	
	grabity_ -= kGrabityPower_;
	//重力
	if (!isGround_) {
		wt_.translation_.y += grabity_;
	}
	else {
		grabity_ = 0.0f;
	}

}

void IEnemy::PlayerTarget() {
	Segment segment;
	segment.origin = wt_.translation_;
	segment.diff = player_->GetTranslate();

	//playerと敵との間に壁があるならば
	for (auto& stage : stages_) {
		if (IsCollisionAABB_Segment(stage, segment)) {
			isFoundTarget_ = false;
			break;
		}
	}
}

void IEnemy::SearchRange() {
	if (wt_.rotation_.y == kDirectionRight_) {
		eyeAABB_.min = wt_.translation_ + Vector3(0, -eyeReach_.y, -eyeReach_.z);
		eyeAABB_.max = wt_.translation_ + eyeReach_;
		speed_.x = kMoveX_;
	}
	else if (wt_.rotation_.y == kDirectionLeft_) {
		eyeAABB_.min = wt_.translation_ + -eyeReach_;
		eyeAABB_.max = wt_.translation_ + Vector3(0, eyeReach_.y, eyeReach_.z);
		speed_.x = -kMoveX_;
	}
}

void IEnemy::MoveEnemy() {
	wt_.translation_ += speed_;
	move_ += speed_;

	//移動ポイントの端だと向きを変える
	//右端に行ったら左に旋回
	if (move_.x > routePointRight_.x) {
		wt_.rotation_.y = kDirectionLeft_;
	}
	//左端に行ったら右に旋回
	if (move_.x < routePointLeft_.x) {
		wt_.rotation_.y = kDirectionRight_;
	}
}

void IEnemy::RespawnEnemyCommon() {
	RespawnCommon();

	isDeleteEnemy_ = false;
	isBullet_ = false;//攻撃はしない

	//向きリセット
	DirectionDegree();
	//移動ルート位置戻す
	move_ = { 0,0,0 };

	//弾はすべて消す
	bullets_.remove_if([](EnemyBullet* bullet) {
		delete bullet;
		return true;
	});
}

void IEnemy::DeadUpdate() {
	if (isDead_) {
		wt_.rotation_.z += 3.0f;
	}
	else {
		wt_.rotation_.z = 0.0f;
		return;
	}

	if (wt_.rotation_.z > 90.0f) {
		isDeleteEnemy_ = true;
	}
}

void IEnemy::DirectionDegree() {

	//0~360にする
	wt_.rotation_.y = std::fmod(wt_.rotation_.y, kMaxAngle);
	//-の場合
	if (wt_.rotation_.y < 0) wt_.rotation_.y += 360.0;

	///0~180は右
	if (wt_.rotation_.y >= 0.0f && wt_.rotation_.y < 180.0f) {
		wt_.rotation_.y = kDirectionRight_;
	}///180~360は左
	else if (wt_.rotation_.y <= kMaxAngle) {
		wt_.rotation_.y = kDirectionLeft_;
	}
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

void IEnemy::ExplosionEnd() {}

void IEnemy::Fire() {
	
	//クールタイム
	coolTime_ += 1.0f / 60.0f;
	if (coolTime_ >= kCoolTimeMax_) {

		//連射で時間を開ける
		rapidFireTime_ += 1.0f / 60.0f;
		if (rapidFireTime_ >= kRapidFireTimeMax_) {
			FireBullet();//敵の発泡攻撃
			particleFire_->SetParticleBorn(ParticleBorn::MomentMode);//パーティクルが出てくる
			rapidCount_++;//カウント
			rapidFireTime_ = 0;//もう一度
		}

		//最大弾丸数を超えた場合
		if (rapidCount_ == rapidCountMax_) {
			rapidCount_ = 0;//カウントリセット
			coolTime_ = 0;//クールタイム発動
			isBullet_ = false;//撃たないフラグ
		}
	}
}

void IEnemy::FireBullet(){}