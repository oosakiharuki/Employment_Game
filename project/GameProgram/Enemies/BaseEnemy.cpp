#include "BaseEnemy.h"
#include "ImGuiManager.h"
#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

BaseEnemy::BaseEnemy() {}

BaseEnemy::~BaseEnemy(){}

void BaseEnemy::Enemy_InitializeCommon(const std::string& objectName) {
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
	//Transform更新処理
	transformMark_ = wtMark_.UpdateTransform();
}

void BaseEnemy::Update() {

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
	if (isDamageMotion_) {
		reaction_->ScaleReaction(transform_.scale,isDamageMotion_, damageScale_, scaleTimer_, kDamageMaxTime_);
	}
	//弾丸更新
	BulletUpdate();

#ifdef USE_IMGUI
	
	//ImGui 更新
	UpdateImGui();

#endif // USE_IMGUI

	//オブジェクト更新
	UpdateBehind();
}

void BaseEnemy::UpdateBehind() {
	//影
	shadow_->SetTranslate(transform_.translate);

	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);

	//設定した全てのパーティクル更新処理
	for (auto& particle : particles_) {
		particle.second->Update();
	}
}

void BaseEnemy::MarkUpdate() {

	//!,?のマーク表示時間の間
	markTimer_ = std::clamp(markTimer_, 0.0f, kMarkMaxTime_);//0 ～ kMarkMaxTime

	// - マーク -
	transformMark_.translate = transform_.translate;  //敵の座標位置に
	transformMark_.translate.y += kMarkPositionY_;//敵の少し上の位置に
	
	//!,?マークの更新処理
	wtMark_.UpdateMatrix(transformMark_);
	objectFound_->Update(wtMark_);
	objectNoFound_->Update(wtMark_);
}

void BaseEnemy::MarkDraw() {
	//見つけたとき+マークが出る時間
	if(isBullet_ && markTimer_ < kMarkMaxTime_)
		objectFound_->Draw();

	//見失った+マークが出る時間
	if (isLostPlayer_ && markTimer_ > 0.0f)
		objectNoFound_->Draw();
}

void BaseEnemy::DrawCommon() {
	if (hp_ == 0) return;
	MarkDraw();
}

void BaseEnemy::DrawParticle() {
	//設定した全てのパーティクル描画処理
	for (auto& particle : particles_) {
		particle.second->Draw();
	}
}

void BaseEnemy::BulletUpdate() {
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


void BaseEnemy::IsDamage() {
	//ダメージのパーティクルを出す
	particles_[particleDamage_.name]->SetTranslate(transform_.translate); //座標を読み取る
	particles_[particleDamage_.name]->SetParticleBorn(ParticleBorn::MomentMode); // 発生モード(一度だけ)の変更
	isDamageMotion_ = true;

	//連続ヒット時、元に戻す
	transform_.scale = kDefaultScale_;
	scaleTimer_ = 0.0f;

	//Hpが0なら
	if (hp_ == 0) {
		return;
	}
	//体力 -1
	hp_--;
}

void BaseEnemy::GravityUpdate() {
	//重力
	gravity_ -= kGravityPower_;
	//地面についていない
	if (!isGround_) {
		transform_.translate.y += gravity_;
	}
	else {
		//重力パワーリセット
		gravity_ = 0.0f;
	}
}

void BaseEnemy::PlayerTarget() {
	//見つかった瞬間だけtrueに
	if (isBullet_ && markTimer_ < kFoundMotionMaxTime_) {
		isFoundReaction_ = true;
		preTranslate_ = transform_.translate;
	}

	Segment segment;
	segment.origin = transform_.translate;      //敵座標
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

void BaseEnemy::SearchRange() {
	//敵が右向き
	if (transform_.rotate.y == kDirectionRight_) {
		eyeAABB_.min = transform_.translate + Vector3(0, -eyeReach_.y, -eyeReach_.z);
		eyeAABB_.max = transform_.translate + eyeReach_;
		speed_.x = kMoveX_;//右に進む
	}
	//左向き
	else if (transform_.rotate.y == kDirectionLeft_) {
		eyeAABB_.min = transform_.translate + -eyeReach_;
		eyeAABB_.max = transform_.translate + Vector3(0, eyeReach_.y, eyeReach_.z);
		speed_.x = -kMoveX_;//左に進む
	}
}

void BaseEnemy::MoveEnemy() {
	transform_.translate += speed_;//移動
	move_ += speed_;           //移動ポイント

	//移動ポイントの端だと向きを変える
	//右端に行ったら左に旋回
	if (move_.x > routePointRight_.x) {
		transform_.rotate.y = kDirectionLeft_;
	}
	//左端に行ったら右に旋回
	if (move_.x < routePointLeft_.x) {
		transform_.rotate.y = kDirectionRight_;
	}
}

void BaseEnemy::RespawnEnemyCommon() {
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

void BaseEnemy::DirectionDegree() {

	//360度以上の場合 [360除算のあまり]
	transform_.rotate.y = std::fmod(transform_.rotate.y, kMaxAngle);
	//-の場合 [0以上になるまで360加算]
	while (transform_.rotate.y < 0) {
		transform_.rotate.y += kMaxAngle;
	}

	///0~179は右
	if (transform_.rotate.y >= 0.0f && transform_.rotate.y < kMaxAngle * kDivideByTwo_) {
		transform_.rotate.y = kDirectionRight_;
	}///180~360は左
	else if (transform_.rotate.y <= kMaxAngle) {
		transform_.rotate.y = kDirectionLeft_;
	}
}

AABB BaseEnemy::GetBombAABB() { 
	AABB null{};
	return null;
}

Vector3 BaseEnemy::GetDistance(){
	Vector3 null{};
	return null;
}

bool BaseEnemy::IsExplosion() { return false; }

void BaseEnemy::Fire() {
	
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

void BaseEnemy::FireBullet(){}

void BaseEnemy::FoundReaction() {

	//伸びる強さ(y軸のみ)
	Vector3 reaction = { 0,damageScale_.y * kDivideByTwo_,0 };

	if (isFoundReaction_) {
		reaction_->ScaleReaction(transform_.scale, isFoundReaction_, reaction, scaleTimer_, kFoundMotionMaxTime_);
		reaction_->FoundReaction(transform_.translate,isFoundReaction_, reaction, foundTimer_, kFoundMotionMaxTime_, preTranslate_);
	}
}

void BaseEnemy::DeadReaction() {
	transform_.rotate -= TransformNormal(Vector3{ kDeadRotation_,0,0 }, wt_.GetMatWorld());

	//リアクションフラグ
	bool isReaction = true;

	//伸びる強さ(y軸のみ)
	Vector3 reaction = { 0,damageScale_.y * kDivideByTwo_,0 };
	reaction_->FoundReaction(transform_.translate, isReaction, reaction, foundTimer_, kMarkMaxTime_ * kDivideByTwo_, preTranslate_);

	//リアクションが終わったら
	if (!isReaction) {
		isDeleteEnemy_ = true;
	}
}

bool BaseEnemy::IsLostFound() {
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

void BaseEnemy::EnemyFire() {
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

void BaseEnemy::ChangeStatePattern(std::unique_ptr<BaseEnemyState> enemyState) {
	currentEnemyState_.reset();
	currentEnemyState_ = std::move(enemyState);
}