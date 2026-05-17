/// ------------
///
/// プレイヤー
/// 
/// ------------
#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Primitive.h"

#include "SpriteCommon.h"
#include "SkinningCommon.h"
#include <numbers>

#include "ParticleManager.h"
#include <NextStageSave.h>
#include "FoldingUmbrella.h"
#include <TimeScale.h>

using namespace MyMath;
using namespace UseEveryOne;

Player::Player(){}

Player::~Player() {}

void Player::Initialize() {
	//アクターの共通初期化処理
	GameActor::Initialize();

	//アニメーションを保管
	InitMainBody(); 
	
	//傘の初期化
	InitUmbrella();

	//SE
	InitAudio();

	//パーティクル初期化
	InitParticles();

	//MaxHp初期設定
	maxHp_ = kPlayerMaxHp_;

	//UI_体力
	for (uint32_t i = 0; i < maxHp_; i++) {
		SettingSpriteHp(i);
	}

	//強化ゲージ
	reinforceGauge_ = std::make_unique<ReinforceGauge>();
	reinforceGauge_->Initialize();

	//コマンドパターン
	playerActionsInputHandler_ = std::make_unique<PlayerActionsInputHandler>();

	//コリジョンタイプ
	collisionType_ = CollisionTypes::TypePlayer;

	eventMin = -kMoveMax_;
	eventMax = kMoveMax_;
}

void Player::InitMainBody() {
	objectMotions_["standby"] = "player_standby.gltf";
	objectMotions_["move"] = "player_move.gltf";
	objectMotions_["shield"] = "player_shield.gltf";
	objectMotions_["clear"] = "player_clear.gltf";
	objectMotions_["appearance"] = "player_appearance.gltf";

	//プレイヤー初期化/オブジェクト読み込み
	object_ = std::make_unique<EngineLayer::Object_glTF>();
	object_->Initialize();
	object_->SetModelFile(objectMotions_[motionName_]);
}


void Player::InitParticles() {
	particles_[particleWalk_] = EngineLayer::ParticleManager::GetInstance().InitParticle(particleWalk_);
	particles_[particleJump_] = EngineLayer::ParticleManager::GetInstance().InitParticle(particleJump_);
	particles_[particleBrink_] = EngineLayer::ParticleManager::GetInstance().InitParticle(particleBrink_);
	particles_[particleFire_] = EngineLayer::ParticleManager::GetInstance().InitParticle(particleFire_);
	particles_[particleDamage_] = EngineLayer::ParticleManager::GetInstance().InitParticle(particleDamage_);
	particles_[particleParry_] = EngineLayer::ParticleManager::GetInstance().InitParticle(particleParry_);
	particles_[particleDead_] = EngineLayer::ParticleManager::GetInstance().InitParticle(particleDead_);

}

void Player::InitUmbrella() {
	umbrella_ = std::make_unique<Umbrella>();
	umbrella_->Initialize();

	//傘の行列
	wtGun_.Initialize();
	//Transform更新処理
	transformGun_ = wt_.UpdateTransform();
	//プレイヤー情報
	umbrella_->SetPlayer(this);
}

void Player::SettingSpriteHp(uint32_t num) {
	std::unique_ptr<EngineLayer::Sprite> sprite = std::make_unique<EngineLayer::Sprite>();
	sprite->Initialize("Hp.png");
	sprite->SetPosition({ kInitializePointHp_.x + kTextureSizeHp_.x * num , kInitializePointHp_.y - num * kDistanceYHp_ });
	sprite->SetSize(kTextureSizeHp_);

	hpSprites_.push_back(std::move(sprite));
}

void Player::InitAudio() {
	//ダメージ
	EngineLayer::Audio::GetInstance().LoadWave(kHitSoundName_);
	//ジャンプ
	EngineLayer::Audio::GetInstance().LoadWave(kJumpSoundName_);
}

void Player::ActionUpdate() {
	//コマンドを受け取る
	playerActionsInputHandler_->GetCommand(this,command_);
	//コマンドが入っているなら
	for (auto& command : command_) {
		command->SetPlayer(this);
		//コマンド出力
		command->Execute();
	}
	command_.clear();
}


void Player::Update() {
	GameActor::Update();

	//弾丸更新処理
	//消滅処理
	bullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			bullet.reset();
			return true;
		}
		return false;
		});
	//弾丸
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	//発砲のクールタイム
	fireCoolTimer_ -= kDeltaTime_;
	fireCoolTimer_ = std::clamp(fireCoolTimer_, 0.0f, kFireCoolTimeMax_);
	

	//無敵時間
	InfinityTimeUpdate();

	//影の更新
	ShadowUpdate();

	//リアクション
	ReactionsUpdate();

	//アニメーション更新処理
	AnimationUpdate();

	//パーティクル
	for (auto& p : particles_) {
		if(p.second)
		p.second->Update();
	}

	//土煙のパーティクル
	SmockParticle();

	//移動量を加算
	CollisionUtility::GetInstance().OnMoveGround(transform_.translate);

	//現在座標に前回座標を代入
	prePosition_ = transform_.translate;

	//imGui更新処理
	ImGuiUpdate();

	//最後のほうにする更新処理(オブジェクト更新,WorldTransform更新)
	BehindUpdate();

	//スプライト更新
	SpriteUpdate();

	//当たり判定設定
	collisionAABB_.min = transform_.translate - colliderSize_;
	collisionAABB_.max = transform_.translate + colliderSize_;

	center_ = transform_.translate;//真ん中の座標

	CollisionManager::GetInstance().FrameCollision(this);
	collisionOverlap = CollisionUtility::GetInstance().SetTarget(GetTranslate(), GetAABB());
	isGround_ = false;

}

void Player::InfinityTimeUpdate() {
	if (infinityTimer_ >= kInfinityTimeMax_) {
		infinityTimer_ = kInfinityTimeMax_;//Maxになったら無敵時間終了
		TimeScale::GetInstance().SetTimeScale(kDeltaTime_);
		object_->SetColor({ 1.0f,1.0f,1.0f,1.0f });
		return;
	}
	else if (BrinkTimeMax()) {
		infinityTimer_ += TimeScale::GetInstance().GetTimeScale();//時間が経過する
	}
	else {
		infinityTimer_ += kDeltaTime_;//時間が経過する
	}

	//
	if (std::fmod(infinityTimer_, kBlinkingTime_) >= kBlinkingTime_ - kDeltaTime_) {
		isChangeColor_ = !isChangeColor_;
	}

	if(isChangeColor_) {
		object_->SetColor({0.5f,0.5f,0.5f,1.0f});
	}
	else {
		object_->SetColor(kDefaultColor_);
	}
}

void Player::ReactionsUpdate() {
	//ダメージリアクション
	reaction_->ScaleReaction(transform_.scale, isDamageMotion_, damageScale_, scaleTimer_, kDamageMaxTime_);
}

void Player::AnimationUpdate() {
	///アニメーション
	if (umbrella_->GetShieldMode()) {
		motionName_ = "shield";
	}//前回の座標と現在の座標が違う = 動いた場合 + 空中
	else if (IsMovePosition() || !isGround_) {
		motionName_ = "move";
	}
	else {
		motionName_ = "standby";
	}

	if (isPerformance_) {
		motionName_ = "clear";
		//タイマーが進まない場合
		if (appearanceAnimationTimer_ > 0.0f) {
			motionName_ = "appearance";
		}
	}

	//animationが変わった場合切り替える
	if (motionName_ != preMotionName_) {
		object_->ChangeAnimation(objectMotions_[motionName_]);
		preMotionName_ = motionName_;
	}
}

void Player::SmockParticle() {
	//移動しているとパーティクルを発生
	if (isGround_ && IsMovePosition()) {
		// 歩く煙パーティクル
		particles_[particleWalk_]->SetParticleBorn(EngineLayer::ParticleBorn::TimerMode);
		particles_[particleWalk_]->SetTranslate(transform_.translate + TransformNormal(kParticleWalkPoint_, wt_.GetMatWorld()));
	}
	else {
		particles_[particleWalk_]->SetParticleBorn(EngineLayer::ParticleBorn::Stop);
	}
}

void Player::ImGuiUpdate() {
#ifdef  USE_IMGUI

	ImGui::Begin("player");

	ImGui::InputFloat3("worldTransform.translate", &transform_.translate.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &transform_.translate.x, -30.0f, 30.0f);

	ImGui::InputFloat3("Rotate", &transform_.rotate.x);
	ImGui::SliderFloat("RotateX", &transform_.rotate.x, -kMaxAngle, kMaxAngle);
	ImGui::SliderFloat("RotateY", &transform_.rotate.y, -kMaxAngle, kMaxAngle);
	ImGui::SliderFloat("RotateZ", &transform_.rotate.z, -kMaxAngle, kMaxAngle);

	ImGui::Text("体力:%d", hp_);
	ImGui::Text("残機 %d", remain_);

	ImGui::End();

#endif //  USE_IMGUI
}

void Player::BehindUpdate() {

	wt_.UpdateMatrix(transform_);
	object_->Update(wt_);

	wtGun_.UpdateMatrix(transformGun_);
	// - 傘の銃 -
	//プレイヤーの手前に
	umbrella_->SetTranslate(transform_.translate +
		TransformNormal(kPlayerFront_, wtGun_.GetMatWorld()));
	//傘の方向
	umbrella_->SetRotate(transformGun_.rotate);
	//更新
	umbrella_->Update();
}


void Player::LifeUpdate() {

	//傘の方向を読み取る
	umbrellaRange_ = transformGun_.rotate;
	//円柱または円錐が縦のため、90度回転して横にする
	umbrellaRange_.x += kNinetyAngle_;

	//ノックバック発動
	KnockBackUpdate();

	//ジャンプによる変動
	//地面にいるとき
	if (GetIsGround()) {
		jumpPower_ = 0.0f;//ジャンプ可能
		isOneBrink_ = false;//ブリンク可能
	}
	else {
		transform_.translate.y += jumpPower_;
	}

	//重力
	GravityUpdate(transform_.translate.y);

	//落ちた場合
	IsFall();

	if (CollisionUtility::GetInstance().IsGoal() || CollisionUtility::GetInstance().IsWarp()) {
		isPerformance_ = true;
	}
}


void Player::Active() {
	//プレイヤー操作
	//アクションステートの更新処理
	ActionUpdate();

	//生きている状態の更新処理
	LifeUpdate();

	if (!isEvent_) {
		//イベント範囲解放
		eventMin = -kMoveMax_;
		eventMax = kMoveMax_;
	}
	else {
		//動ける範囲制限
		transform_.translate.x = std::clamp(transform_.translate.x, eventMin.x, eventMax.x);
		transform_.translate.y = std::clamp(transform_.translate.y, eventMin.y, eventMax.y);
		transform_.translate.z = std::clamp(transform_.translate.z, eventMin.z, eventMax.z);
	}
	isEvent_ = false;
}

void Player::Dead() {
	//ノックバック、ダメージリアクション、ブリンクをリセット
	isKnockback_ = false;
	isDamageMotion_ = false;

	deadTimer_ += kDeltaTime_;

	//少しディレイを挟む(カメラのシェイクが終わったら)
	if (deadTimer_ >= kHitStopTime_) {
		//倒されたパーティクル配置+発動
		particles_[particleDead_]->SetTranslate(transform_.translate);
		particles_[particleDead_]->SetParticleBorn(EngineLayer::ParticleBorn::TimerMode);

		DirectionTheCamera();//カメラのほうに向く
		transform_.rotate.z += kPlayerDeadRotating_;//回転する
		//少し浮く
		transform_.translate.y += kDeadLittleUp_;
		//重力
		GravityUpdate(transform_.translate.y,true);

		isGround_ = false;
		if (deadTimer_ >= kDeadTimeMax_) {
			RespawnPlayer();
			//パーティクル発動停止
			particles_[particleDead_]->SetParticleBorn(EngineLayer::ParticleBorn::Stop);
		}
	}
	else {
		//止まっているので発動しないようにする
		gravity_ = 0.0f;
		isGround_ = true;
	}
}

void Player::Performance() {

	if (CollisionUtility::GetInstance().IsGoal()) {
		DirectionTheCamera();//向きをカメラのほうに(-Z方向)
	}
	else if (CollisionUtility::GetInstance().IsWarp()) {
		BackDirection();//向きを前に(Z方向)
		appearanceAnimationTimer_ = 0.0f;
	}	
	else {
		appearanceAnimationTimer_ += kDeltaTime_;
		//水たまりから出てくるスタート演出処理
		if (appearanceAnimationTimer_ >= appearanceAnimationFinishTime_) {
			IsPerformanceFlag(false);//演出モードを終了し操作できるように
			appearanceAnimationTimer_ = 0.0f;

			//疑似ジャンプ演出
			jumpPower_ = 0.3f;
			IsGround(false);
			transform_.translate.y += jumpPower_;
			ParticleJump();
		}

	}	
}

#pragma region プレイヤーの操作

void Player::KnockBackUpdate() {
	if (isKnockback_) {
		//ゼロならイーズインされない
		if (knockBackTimeMax_ == 0.0f) {
			transform_.translate -= backPower_;
			isKnockback_ = false;
		}
		else {
			knockBackTimer_ += kDeltaTime_;
			//backPower分マイナス
			transform_.translate = EaseOut(transform_.translate, transform_.translate - backPower_, knockBackTimer_);
			if (knockBackTimer_ >= knockBackTimeMax_) {
				isKnockback_ = false;
				knockBackTimer_ = 0.0f;
			}
		}
	}
}

void Player::WeaponChangeUmbrella(std::unique_ptr<BaseUmbrella> nextUmbrella, uint32_t num) {
	//同じなら変更なし
	if (weaponNum_ == num) {
		return;
	}
	//傘の再設定
	umbrella_.reset();
	umbrella_ = std::move(nextUmbrella);//傘のクラス
	umbrella_->Initialize();//初期化
	umbrella_->SetPlayer(this);//プレイヤークラスを設定

	//番号を変更
	weaponNum_ = num;
}

void Player::AddBullet(std::unique_ptr<PlayerBullet> bullet) {
	bullets_.push_back(std::move(bullet));
}


#pragma endregion

void Player::Draw() {
	EngineLayer::GLTFCommon::GetInstance().Command();
	//プレイヤー本体
	object_->Draw();

	if (hp_ != 0 && !isPerformance_) {
		//傘
		umbrella_->Draw();
		//影
		shadow_->Draw();
	}
	
	EngineLayer::Object3dCommon::GetInstance().Command();

	//弾丸
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void Player::DrawParticle() {
	//パーティクル
	for (auto& particle : particles_) {
		if (particle.second)
		particle.second->Draw();
	}
}


Vector3 Player::GetWorldPosition()  {
	Vector3 worldPos;

	worldPos.x = wt_.GetMatWorld().m[3][0];
	worldPos.y = wt_.GetMatWorld().m[3][1];
	worldPos.z = wt_.GetMatWorld().m[3][2];

	return worldPos;
}

void Player::OnUmbrellaShield() {
	umbrella_->ShieldMode();
}

void Player::OffUmbrellaShield() {
	umbrella_->OffShield();
}

void Player::FireBulletUmbrella() {
	// クールタイムは終了した時
	if (fireCoolTimer_ == 0.0f) {
		isFiring_ = true;
		umbrella_->FireCommand();
	}

	if (!isFiring_ && fireCoolTimer_ == 0.0f) {
		fireCoolTimer_ = kFireCoolTimeMax_;
	}
}

void Player::OnCollision(CollisionSource* collision) {		
	if ((collision->GetType() == CollisionTypes::TypeEnemyBullet ||
		collision->GetType() == CollisionTypes::TypeBombExplotion ||
		collision->GetType() == CollisionTypes::TypeEnemyDamageBody ||
		collision->GetType() == CollisionTypes::TypeBoss) && infinityTimer_ >= kInfinityTimeMax_) {
		EnemyCollision(collision);
	}

	if (collision->GetType() == CollisionTypes::TypeStage || collision->GetType() == CollisionTypes::TypeMoveGround) {
		CollisionUtility::GetInstance().GameActorAndStageCollision(collisionOverlap,*this, *this,collision->GetAABB());
	}

	if (collision->GetType() == CollisionTypes::TypeEvent) {
		//イベント範囲から出れないように
		eventMin = collision->GetAABB().min + transform_.scale;
		eventMax = collision->GetAABB().max - transform_.scale;
		isEvent_ = true;
	}

	//チャージポイントに触れるとゲージがマックスになる
	if (collision->GetType() == CollisionTypes::TypeEnergy) {
		reinforceGauge_->AddPoint();
	}
}

bool Player::TypeCheckUp(const CollisionTypes& collisionType) {
	//体力0(死亡)+演出状態の時は当たらない
	if (hp_ == 0 || isPerformance_) {
		return false;
	}

	if (collisionType == CollisionTypes::TypeEnemyBullet ||
		collisionType == CollisionTypes::TypeEnemyDamageBody ||
		collisionType == CollisionTypes::TypeBombExplotion ||
		collisionType == CollisionTypes::TypeBoss ||
		collisionType == CollisionTypes::TypeStage || 
		collisionType == CollisionTypes::TypeMoveGround ||
		collisionType == CollisionTypes::TypeEvent || 
		collisionType == CollisionTypes::TypeEnergy) {
		return true;
	}
	return false;
}

void Player::EnemyCollision(CollisionSource* collision) {
	//ブリンク時間半分たつ前 + 時間がスロー状態でないとき
	if (brinkTimer_ >= kBrinkTimeMax_ * kDivideByTwo_ && TimeScale::GetInstance().GetTimeScaleFacto() == 1.0f) {
		TimeScale::GetInstance().SetTimeScale(1.0f / 180.0f);
		InfinityTime();//無敵時間が入る
		//ポイントを6プラス(ゲージ2個分)
		for (int i = 0; i < 6; i++) {
			reinforceGauge_->AddPoint();
		}
	}
	else {
		//ダメージ
		IsDamage(collision->GetCenter());
	}
}

void Player::IsDamage(const Vector3& hitPoint) {
	//無敵時間をすぎたとき
	if (infinityTimer_ >= kInfinityTimeMax_) {
		//体力がもうないなら
		if (hp_ == 0) {
			return;
		}
		//体力 -1
		hp_--;
		//ダメージのパーティクル発生
		particles_[particleDamage_]->SetTranslate(transform_.translate + Normalize(hitPoint));
		particles_[particleDamage_]->SetParticleBorn(EngineLayer::ParticleBorn::MomentMode);
		//ダメージのSE再生
		EngineLayer::Audio::GetInstance().SoundPlayWave(kHitSoundName_, kVolume_);
		InfinityTime();
		//ノックバック(時間の三分の一ぶんまで)

		Vector3 power = Length(transform_.translate,hitPoint);

		//当たり所が右の場合
		if (transform_.translate.x > hitPoint.x) {
			power.x = -power.x;
		}//当たり所が上の場合
		if (transform_.translate.y > hitPoint.y) {
			power.y = -power.y;
		}


		KnockBackPlayer(power , kInfinityTimeMax_ * kDivideByThree_);
	}
	//リアクションフラグ
	isDamageMotion_ = true;
}

void Player::IsFall() {
	if (transform_.translate.y >= kFallEndY_) {
		return;//落ちていない
	}
	//落ちる地点より下の場合
	//一発K.O
	hp_ = 0;
	//ダメージSE再生
	EngineLayer::Audio::GetInstance().SoundPlayWave(kHitSoundName_, kVolume_);
}

void Player::KnockBackPlayer(const Vector3& Power, float TimerMax) {
	//威力を代入
	backPower_ = Normalize(Power) * kDivideByTwo_;
	KnockBackCommon(TimerMax);	
	//リアクション
	//連続ヒット時、大きさを元に戻す
	scaleTimer_ = 0.0f;
	transform_.scale = kDefaultScale_;
}

void Player::KnockBackUmbrella(const Vector3& Power, float TimerMax) {
	//威力を代入(傘の向きに沿って)
	backPower_ = TransformNormal(Power, wtGun_.GetMatWorld());
	KnockBackCommon(TimerMax);
	//連続ヒット時、元に戻す
	umbrella_->SetScale(kDefaultScale_);
}

void Player::KnockBackCommon(float TimerMax) {
	backPower_.z = 0.0f;//z方向はなし
	//ノックバックするフラグ
	isKnockback_ = true;
	//ノックバック時間(EaseOutで使用する)
	knockBackTimeMax_ = TimerMax;
}

void Player::RespawnPlayer() {

	if (remain_ == 0) {
		return;
	}
	//残機を減らす
	remain_--;
	//0なら初期位置に戻すなどがいらない
	if (remain_ != 0) {
		hp_ = maxHp_;    //体力を満タンに	
	}

	deadTimer_ = 0.0f;

	//移動範囲を元に戻す
	eventMin = -kMoveMax_;
	eventMax = kMoveMax_;
	isRespawn_ = true;

	transform_.translate = NextStageSave::GetInstance().GetNextStageSaveData().checkPoint;
	transform_.translate.z = 0.0f;
	isGround_ = false;

	transform_.rotate = { 0,180,0 };
}

void  Player::ParticleFire(const Vector3& translate) {
	//攻撃パーティクル発生
	particles_[particleFire_]->SetTranslate(translate);
	particles_[particleFire_]->SetRotate(umbrellaRange_);
	particles_[particleFire_]->SetParticleBorn(EngineLayer::ParticleBorn::MomentMode);
}

void  Player::ParticleJump() {
	particles_[particleJump_]->SetTranslate(transform_.translate + TransformNormal(kParticleWalkPoint_, wt_.GetMatWorld()));
	particles_[particleJump_]->SetParticleBorn(EngineLayer::ParticleBorn::MomentMode);
	//SE
	EngineLayer::Audio::GetInstance().StopWave(kJumpSoundName_);
	EngineLayer::Audio::GetInstance().SoundPlayWave(kJumpSoundName_, kVolume_);
}

void  Player::ParticleBrink() {
	particles_[particleBrink_]->SetTranslate(transform_.translate);
	Vector3 rotate = transformGun_.rotate;
	rotate += TransformNormal(kLookToCameraDirection_,wtGun_.GetMatWorld());
	particles_[particleBrink_]->SetRotate(rotate);
	particles_[particleBrink_]->SetParticleBorn(EngineLayer::ParticleBorn::TimerMode);
}

bool Player::BrinkFlag() {
	if (!isOneBrink_ && UseGaugePoint()) {
		brinkTimer_ = kBrinkTimeMax_;//タイマーを
		return true;
	}
	return false;
}

bool Player::BrinkTimeMax() {
	if (brinkTimer_ > 0.0f) {
		return true;
	}
	return false;
}

void Player::StopParticleBrink() {
	particles_[particleBrink_]->SetParticleBorn(EngineLayer::ParticleBorn::Stop);
}

void Player::GravityDown() {
	//重力を固定することでゆっくり落ちる
	gravity_ = kFixedGravityPower_ * TimeScale::GetInstance().GetTimeScaleFacto();
}

const bool Player::IsMovePosition() {
	if (transform_.translate.x != prePosition_.x) {
		return true;
	}
	return false;
}

void Player::SpriteUpdate() {
	uint32_t nowHp = 0;
	for (auto& sprite : hpSprites_) {
		if (nowHp >= hp_) {
			sprite->SetTextureFile("NoHp.png");
		}//テクスチャ体力ない状態なら変更
		else if (sprite->GetTextureFile() != "Hp.png") {
			sprite->SetTextureFile("Hp.png");
		}

		UIManager::GetInstance().FrameSprite(&*sprite);
		nowHp++;
	}

	//追加の操作
#ifdef _DEBUG
	//強化ゲージ
	if (EngineLayer::Input::GetInstance().PushKey(DIK_T)) {
		reinforceGauge_->AddPoint();
	}
	
	if (EngineLayer::Input::GetInstance().TriggerKey(DIK_Y)) {
		reinforceGauge_->UsePoint();
	}
#endif // _DEBUG
	//強化ゲージの更新
	reinforceGauge_->Update();
}

bool Player::UseGaugePoint() { 
	return reinforceGauge_->UseGaugePoint();
}
