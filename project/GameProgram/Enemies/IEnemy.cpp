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

void IEnemy::Update() {

	//演出中の場合
	if (player_->GetPerformanceMode()) {
		//オブジェクトのみの更新
		UpdateBehind();
		return;
	}

	//hpが0の時
	if (hp_ == 0) {
		isDead_ = true;
	}

	//捜索範囲更新
	SearchRange();

	//ステートパターンの更新処理
	currentEnemyState_->Update(*this);

	//リアクション
	if (isDamageMosion_) {
		reaction_->ScaleReaction(wt_.scale_,isDamageMosion_, damageScale_, scaleTimer_, kDamageMaxTime_);
	}
	//弾丸更新
	BulletUpdate();

#ifdef USE_IMGUI
	
	//Imgui 更新
	UpdateImgui();

#endif // USE_IMGUI

	//オブジェクト更新
	UpdateBehind();
}

void IEnemy::UpdateBehind() {
	//影
	shadow_->SetTranslate(wt_.translation_);

	object_->Update(wt_);
	wt_.UpdateMatrix();

	//設定した全てのパーティクル更新処理
	for (auto& particle : particles_) {
		particle.second->Update();
	}
}

void IEnemy::MarkUpdate() {

	//!,?のマーク表示時間の間
	markTimer_ = std::clamp(markTimer_, 0.0f, kMarkMaxTime_);//0 ～ kMarkMaxTime

	// - マーク -
	wtMark_.translation_ = wt_.translation_;  //敵の座標位置に
	wtMark_.translation_.y += kMarkPositionY_;//敵の少し上の位置に
	
	//!,?マークの更新処理
	wtMark_.UpdateMatrix();
	objectFound_->Update(wtMark_);
	objectNoFound_->Update(wtMark_);
}

void IEnemy::MarkDraw() {
	//見つけたとき+マークが出る時間
	if(isBullet_ && markTimer_ < kMarkMaxTime_)
		objectFound_->Draw();

	//見失った+マークが出る時間
	if (isLostPlayer_ && markTimer_ > 0.0f)
		objectNoFound_->Draw();
}

void IEnemy::DrawCommon() {
	if (hp_ == 0) return;
	MarkDraw();
}

void IEnemy::DrawParticle() {
	//設定した全てのパーティクル描画処理
	for (auto& particle : particles_) {
		particle.second->Draw();
	}
}

void IEnemy::BulletUpdate() {
	//弾丸更新処理
	for (auto& bullet : bullets_) {
		bullet->Update();
	}
	//消滅処理
	bullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			bullet.reset();
			return true;
		}
		return false;
	});
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
	//見つかった瞬間だけtrueに
	if (isBullet_ && markTimer_ < kFoundMosionMaxTime_) {
		isFoundReaction_ = true;
		preTranslate_ = wt_.translation_;
	}

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
	ChangeStatePattern(std::make_unique<EnemyMoveState>());

	//向きリセット
	DirectionDegree();
	//移動ルート位置戻す
	move_ = { 0,0,0 };

	//弾はすべて消す
	bullets_.remove_if([](auto& bullet) {
		bullet.reset();
		return true;
	});

	markTimer_ = 0.0f;
}

void IEnemy::DirectionDegree() {

	//360度以上の場合 [360除算のあまり]
	wt_.rotation_.y = std::fmod(wt_.rotation_.y, kMaxAngle);
	//-の場合 [0以上になるまで360加算]
	while (wt_.rotation_.y < 0) {
		wt_.rotation_.y += kMaxAngle;
	}

	///0~179は右
	if (wt_.rotation_.y >= 0.0f && wt_.rotation_.y < kMaxAngle * kDivideByTwo_) {
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
	coolTime_ += kDeltaTime_;
	if (coolTime_ >= kCoolTimeMax_) {

		//連射で時間を開ける
		rapidFireTime_ += kDeltaTime_;
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

void IEnemy::FoundRiaction() {

	//伸びる強さ(y軸のみ)
	Vector3 reaction = { 0,damageScale_.y * kDivideByTwo_,0 };

	if (isFoundReaction_) {
		reaction_->ScaleReaction(wt_.scale_, isFoundReaction_, reaction, scaleTimer_, kFoundMosionMaxTime_);
		reaction_->FoundReaction(wt_.translation_,isFoundReaction_, reaction, foundTimer_, kFoundMosionMaxTime_, preTranslate_);
	}
}

void IEnemy::DeadReaction() {
	wt_.rotation_ -= TransformNormal(Vector3{ kDeadRotation_,0,0 }, wt_.matWorld_);

	//リアクションフラグ
	bool isReaction = true;

	//伸びる強さ(y軸のみ)
	Vector3 reaction = { 0,damageScale_.y * kDivideByTwo_,0 };
	reaction_->FoundReaction(wt_.translation_, isReaction, reaction, foundTimer_, kMarkMaxTime_ * kDivideByTwo_, preTranslate_);

	//リアクションが終わったら
	if (!isReaction) {
		isDeleteEnemy_ = true;
	}
}

bool IEnemy::IsLostFound() {
	//見失ってから少したってから通常処理に戻す
	if (isLostPlayer_ && markTimer_ <= 0.0f) {
		isLostPlayer_ = false;//リセット
		return true;
	}
	//途中でまた見つかったら
	else if (isLostPlayer_ && isFoundTarget_) {
		isLostPlayer_ = false;//リセット
		markTimer_ = 0.0f;    //タイマーリセット(!マークとリアクションをさせる)
	}
	return false;
}

void IEnemy::EnemyFire() {
	if (isFoundTarget_) {
		isBullet_ = true;
	}

	if (isBullet_) {
		//発泡処理
		Fire();
		//!マーク表示時間
		markTimer_ += kDeltaTime_;
	}
	else if (!isBullet_) {
		//連射タイマー、クールタイマーリセット
		rapidCount_ = 0;
		rapidFireTime_ = 0;
		coolTime_ = 0;
		//見失うフラグ
		isLostPlayer_ = true;
		//?マーク表示時間
		markTimer_ -= kDeltaTime_;
	}

}

void IEnemy::ChangeStatePattern(std::unique_ptr<BaseEnemyState> enemyState) {
	currentEnemyState_.reset();
	currentEnemyState_ = std::move(enemyState);
}