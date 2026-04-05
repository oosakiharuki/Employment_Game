#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Primitive.h"

#include "SpriteCommon.h"
#include "SkinningCommon.h"
#include <numbers>

#include "ParticleManager.h"
#include <NextStageSave.h>

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

	actionCommand_ = std::make_unique<PlayerCommand>();
	actionCommand_->SetPlayer(this);

	//ステートパターン
	actionState_ = std::make_unique<PlayerNormalState>();

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
	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile(objectMotions_[motionName_]);
}


void Player::InitParticles() {
	particles_[particleWalk_] = ParticleManager::GetInstance().InitParticle(particleWalk_);
	particles_[particleBrink_] = ParticleManager::GetInstance().InitParticle(particleBrink_);
	particles_[particleFire_] = ParticleManager::GetInstance().InitParticle(particleFire_);
	particles_[particleDamage_] = ParticleManager::GetInstance().InitParticle(particleDamage_);
	particles_[particleParry_] = ParticleManager::GetInstance().InitParticle(particleParry_);
	particles_[particleDead_] = ParticleManager::GetInstance().InitParticle(particleDead_);

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
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	sprite->Initialize("Hp.png");
	sprite->SetPosition({ kInitializePointHp_.x + kTextureSizeHp_.x * num , kInitializePointHp_.y - num * kDistanceYHp_ });
	sprite->SetSize(kTextureSizeHp_);

	hpSprites_.push_back(std::move(sprite));
}

void Player::InitAudio() {
	//ダメージ
	hitSound_ = Audio::GetInstance().LoadWave("resource/Sound/damage.wav");
	//パリィ
	parrySound_ = Audio::GetInstance().LoadWave("resource/Sound/bane.wav");
}

void Player::ActionUpdate() {
	actionState_->Update(*actionCommand_);
	actionState_->CommandInput(*actionCommand_);

	if (actionState_->GetIsInput()) {
		ChangeStatePatternAction(actionState_->GetNextState());
	}
}


void Player::Update() {
	GameActor::Update();

	//弾丸更新処理
	actionCommand_->BulletUpdate();

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
		p.second->Update();
	}

	//土煙のパーティクル
	SmockParticle();

	//現在座標に前回座標を代入
	prePosition_ = transform_.translate;

	//移動床に乗った時のの処理
	OnMoveGround();

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
	}
	else {
		infinityTimer_ += kDeltaTime_;//時間が経過する
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
	else if (actionCommand_->IsMovePosition() || !isGround_) {
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
	if (isGround_ && actionCommand_->IsMovePosition()) {
		// 歩く煙パーティクル
		particles_[particleWalk_]->SetParticleBorn(ParticleBorn::TimerMode);
		particles_[particleWalk_]->SetTranslate(transform_.translate + TransformNormal(Vector3{ 0.0f,-1.0f,-0.3f }, wt_.GetMatWorld()));
	}
	else {
		particles_[particleWalk_]->SetParticleBorn(ParticleBorn::Stop);
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
	actionCommand_->JumpUpdate();


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
		particles_[particleDead_]->SetParticleBorn(ParticleBorn::TimerMode);

		DirectionTheCamera();//カメラのほうに向く
		transform_.rotate.z += kPlayerDeadRotating_;//回転する
		//少し浮く
		transform_.translate.y += kDeadLittleUp_;
		//重力
		GravityUpdate(transform_.translate.y);

		isGround_ = false;
		if (deadTimer_ >= kDeadTimeMax_) {
			RespawnPlayer();
			//パーティクル発動停止
			particles_[particleDead_]->SetParticleBorn(ParticleBorn::Stop);
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
			isGround_ = true;
			actionCommand_->CommandJump();	
			//ジャンプによる変動
			actionCommand_->JumpUpdate();
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
#pragma endregion

void Player::Draw() {
	GLTFCommon::GetInstance().Command();
	//プレイヤー本体
	object_->Draw();

	if (hp_ != 0 && !isPerformance_) {
		//傘
		umbrella_->Draw();
		//影
		shadow_->Draw();
	}
	
	Object3dCommon::GetInstance().Command();

	//弾丸
	actionCommand_->BulletDraw();
}

void Player::DrawParticle() {
	//パーティクル
	for (auto& particle : particles_) {
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


void Player::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::TypeEnemyBullet || 
		collision->GetType() == CollisionTypes::TypeBombExplotion || 
		collision->GetType() == CollisionTypes::TypeBoss) {
		IsDamage(collision->GetCenter());
	}
	//演出中、死亡の時は当たらない
	if (collision->GetType() == CollisionTypes::TypeStage) {
		CollisionUtility::GetInstance().GameActorAndStageCollision(collisionOverlap,*this, *this,collision->GetAABB());
	}

	if (collision->GetType() == CollisionTypes::TypeMoveGround) {
		move_ = collision->GetCenter();//現在の位置
		CollisionUtility::GetInstance().GameActorAndStageCollision(collisionOverlap, *this, *this, collision->GetAABB());

		//当たった初回
		if (preMove_ == Vector3(0, 0, 0)) {
			preMove_ = move_;//同じにすることで移動量をなしにする
		}		

		//移動量を計算
		value_ = move_ - preMove_;

		//乗っている場合
		if (collisionOverlap.isGround && !Input::GetInstance().TriggerKey(DIK_SPACE)) {
			//当たり判定で離れないように
			value_.y -= kMoveGroundUnder_;
		}
		isMoveGround_ = true;
	}

	if (collision->GetType() == CollisionTypes::TypeEvent) {
		//イベント範囲から出れないように
		eventMin = collision->GetAABB().min + transform_.scale;
		eventMax = collision->GetAABB().max - transform_.scale;
		isEvent_ = true;
	}
}

bool Player::TypeCheckUp(const CollisionTypes& collisionType) {
	//体力0+演出状態の時
	if (hp_ == 0 || isPerformance_) {
		return false;
	}

	if (collisionType == CollisionTypes::TypeEnemyBullet ||
		collisionType == CollisionTypes::TypeBombExplotion ||
		collisionType == CollisionTypes::TypeBoss ||
		collisionType == CollisionTypes::TypeStage || 
		collisionType == CollisionTypes::TypeMoveGround ||
		collisionType == CollisionTypes::TypeEvent) {
		return true;
	}
	return false;
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
		particles_[particleDamage_]->SetParticleBorn(ParticleBorn::MomentMode);
		//ダメージのSE再生
		Audio::GetInstance().SoundPlayWave(hitSound_, kVolume_);
		infinityTimer_ = 0.0f;//無敵時間発動
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
	Audio::GetInstance().SoundPlayWave(hitSound_, kVolume_);
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
	//スタート演出の値設定
	playerPoint_ = transform_.translate;
	pointY_ = kStartPointY_;
	isGround_ = false;
	//ノーマルステートに戻す
	ChangeStatePatternAction(std::make_unique<PlayerNormalState>());
	
	actionCommand_.reset();
	actionCommand_ = std::make_unique<PlayerCommand>();
	actionCommand_->SetPlayer(this);

	transform_.rotate = { 0,180,0 };
}

//パリィ成功
void Player::ParrySuccess() {
	//SE
	Audio::GetInstance().StopWave(parrySound_);//パリィが続くとき一度止めてから再生させるようにする
	Audio::GetInstance().SoundPlayWave(parrySound_, kVolume_);//SE再生:パリィ
	//傘の座標を読み取る
	Vector3 translate = umbrella_->GetTranslate();
	translate += TransformNormal(kPlayerFront_, wtGun_.GetMatWorld());//出す場所をwtGun_の向きの前に
	particles_[particleParry_]->SetTranslate(translate);
	particles_[particleParry_]->SetRotate(umbrellaRange_);
	particles_[particleParry_]->SetParticleBorn(ParticleBorn::MomentMode);
}


void  Player::ParticleFire(const Vector3& translate) {
	//攻撃パーティクル発生
	particles_[particleFire_]->SetTranslate(translate);
	particles_[particleFire_]->SetRotate(umbrellaRange_);
	particles_[particleFire_]->SetParticleBorn(ParticleBorn::MomentMode);
}

void  Player::ParticleBrink() {
	Vector3 translate = GetTranslate() + TransformNormal(-kPlayerFront_, GetUmbrellaMatWorld());
	particles_[particleBrink_]->SetTranslate(translate);
	particles_[particleBrink_]->SetRotate(umbrellaRange_);
	particles_[particleBrink_]->SetParticleBorn(ParticleBorn::MomentMode);
}

void Player::GravityDown() {
	//重力を固定することでゆっくり落ちる
	gravity_ = kFixedGravityPower_;
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
	if (Input::GetInstance().TriggerKey(DIK_T)) {
		reinforceGauge_->AddPoint();
	}
	
	if (Input::GetInstance().TriggerKey(DIK_Y)) {
		reinforceGauge_->UsePoint();
	}
#endif // _DEBUG
	//強化ゲージの更新
	reinforceGauge_->Update();
}

void Player::OnMoveGround() {
	if (isMoveGround_) {
		transform_.translate += value_;//移動した分加算
		preMove_ = move_;//現在の位置を前回位置として使う
	}
	else {
		//リセット
		move_ = { 0,0,0 };
		preMove_ = move_;
	}
	isMoveGround_ = false;
}


void Player::ChangeStatePatternAction(std::unique_ptr<BasePlayerState> playerState) {
	actionState_.reset();
	actionState_ = std::move(playerState);
}

bool Player::UseGaugePoint() { 
	return reinforceGauge_->UseGaugePoint();
}
