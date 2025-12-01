#include "IEnemy.h"
#include "ImGuiManager.h"
#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

IEnemy::IEnemy() {}

IEnemy::~IEnemy(){}

void IEnemy::Enemy_InitializeCommon(const std::string& objectName) {
	//アクターの共通初期化処理
	Actor_InitializeCommon();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile(objectName);

	//見つけたときの「!」マーク
	objectFound_ = std::make_unique<Object3d>();
	objectFound_->Initialize();
	objectFound_->SetModelFile("player_found_mark.obj");

	//見失ったときのの「?」マーク
	objectNoFound_ = std::make_unique<Object3d>();
	objectNoFound_->Initialize();
	objectNoFound_->SetModelFile("player_lost_mark.obj");

	//パーティクル
	//攻撃(発泡)
	particles_[particleFire_.name] = ParticleManager::GetInstance()->InitParticle(particleFire_);
	//ダメージ
	particles_[particleDamage_.name] = ParticleManager::GetInstance()->InitParticle(particleDamage_);

	//マークのワールド座標
	wtMark_.Initialize();
}

void IEnemy::UpdateCommon() {
	if (hp_ == 0) {
		isDead_ = true;
	}

	//死んだときの処理
	DeadUpdate();

	//死んでいない、演出中でないとき
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
			markTimer_ += kDeltaTime_;
		}
		else if(!isBullet_){
			//攻撃、見つけたマークのタイマーリセット
			rapidCount_ = 0;
			rapidFireTime_ = 0;
			coolTime_ = 0;
			isLostPlayer_ = true;//見失うフラグ
			markTimer_ -= kDeltaTime_;
		}		
				
		if (isLostPlayer_ && markTimer_ <= 0.0f)
			isLostPlayer_ = false;
	}

	shadow_->SetTranslate(wt_.translation_);

	if (isDamageMosion_) {
		ScaleUpdate(isDamageMosion_, damageScale_, kDamageMaxTime_);
	}

	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	bullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			bullet.reset();
			return true;
		}
		return false;
		});


}

void IEnemy::UpdateBehind() {
	object_->Update(wt_);
	wt_.UpdateMatrix();

	//設定した全てのパーティクル更新処理
	for (auto& particle : particles_) {
		particle.second->Update();
	}

	if (isDead_) return;//死んでるなら読み取らない

	// - マーク -
	wtMark_.translation_ = wt_.translation_;  //敵の座標位置に
	wtMark_.translation_.y += kMarkPositionY_;//敵の少し上の位置に
	
	//!,?マークの更新処理
	wtMark_.UpdateMatrix();
	objectFound_->Update(wtMark_);
	objectNoFound_->Update(wtMark_);
}

void IEnemy::DrawCommon() {
	if (isDead_) return;//死んでるなら読み取らない

	//見つけたとき+マークが出る時間
	if(isBullet_ && markTimer_ < kMarkMaxTime_)
		objectFound_->Draw();

	//見失った+マークが出る時間
	if (isLostPlayer_ && markTimer_ > 0.0f)
		objectNoFound_->Draw();
}

void IEnemy::DrawParticle() {
	//設定した全てのパーティクル描画処理
	for (auto& particle : particles_) {
		particle.second->Draw();
	}
}

void IEnemy::IsDamage() {
	//ダメージのパーティクルを出す
	particles_[particleDamage_.name]->SetTranslate(wt_.translation_); //座標を読み取る
	particles_[particleDamage_.name]->SetParticleBorn(ParticleBorn::MomentMode); // 発生モード(一度だけ)の変更
	isDamageMosion_ = true;

	//連続ヒット時、元に戻す
	wt_.scale_ = kDefaultScale_;
	scaleTimer_ = 0.0f;

	//Hpが0なら
	if (hp_ == 0) {
		return;
	}
	//体力 -1
	hp_--;
}


void IEnemy::GrabityUpdate() {
	//重力
	grabity_ -= kGrabityPower_;
	//地面についていない
	if (!isGround_) {
		wt_.translation_.y += grabity_;
	}
	else {
		//重力パワーリセット
		grabity_ = 0.0f;
	}

}

void IEnemy::PlayerTarget() {
	Segment segment;
	segment.origin = wt_.translation_;      //敵座標
	segment.diff = player_->GetTranslate(); //プレイヤー座標

	//ステージの当たり判定
	for (auto& stage : stages_) {
		//playerと敵との間に壁があるならば
		if (IsCollisionAABB_Segment(stage, segment)) {
			isFoundTarget_ = false;//見つかってないフラグ
			break;
		}
	}
}

void IEnemy::SearchRange() {
	//敵が右向き
	if (wt_.rotation_.y == kDirectionRight_) {
		eyeAABB_.min = wt_.translation_ + Vector3(0, -eyeReach_.y, -eyeReach_.z);
		eyeAABB_.max = wt_.translation_ + eyeReach_;
		speed_.x = kMoveX_;//右に進む
	}
	//左向き
	else if (wt_.rotation_.y == kDirectionLeft_) {
		eyeAABB_.min = wt_.translation_ + -eyeReach_;
		eyeAABB_.max = wt_.translation_ + Vector3(0, eyeReach_.y, eyeReach_.z);
		speed_.x = -kMoveX_;//左に進む
	}
}

void IEnemy::MoveEnemy() {
	wt_.translation_ += speed_;//移動
	move_ += speed_;           //移動ポイント

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
	//ゲームアクターの共通リスポーン処理
	RespawnCommon();

	isDeleteEnemy_ = false;
	isBullet_ = false;//攻撃はしない

	//向きリセット
	DirectionDegree();
	//移動ルート位置戻す
	move_ = { 0,0,0 };

	//弾はすべて消す
	bullets_.remove_if([](auto& bullet) {
		bullet.reset();
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
			particles_[particleFire_.name]->SetParticleBorn(ParticleBorn::MomentMode);//パーティクルが出てくる
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