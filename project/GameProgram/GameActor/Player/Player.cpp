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

	//ステートパターン
	actionState_ = std::make_unique<PlayerNormalState>();

	//コリジョンタイプ
	collisionType_ = CollisionTypes::player;

	eventMin = -kMoveMax_;
	eventMax = kMoveMax_;
}

void Player::InitMainBody() {
	objectMotions_["standby"] = "player_standby.gltf";
	objectMotions_["move"] = "player_move.gltf";
	objectMotions_["shield"] = "player_shield.gltf";
	objectMotions_["clear"] = "player_clear.gltf";

	//プレイヤー初期化/オブジェクト読み込み
	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile(objectMotions_[motionName_]);
}


void Player::InitParticles() {
	particles_[particleWalk_.name] = ParticleManager::GetInstance().InitParticle(particleWalk_);
	particles_[particleBrink_.name] = ParticleManager::GetInstance().InitParticle(particleBrink_);
	particles_[particleFire_.name] = ParticleManager::GetInstance().InitParticle(particleFire_);
	particles_[particleDamage_.name] = ParticleManager::GetInstance().InitParticle(particleDamage_);
	particles_[particleParry_.name] = ParticleManager::GetInstance().InitParticle(particleParry_);
	particles_[particleDead_.name] = ParticleManager::GetInstance().InitParticle(particleDead_);

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
	SpriteData iterator;
	iterator.name = "playerHp" + std::to_string(num);
	iterator.texturePath = "Hp";
	iterator.position = { kInitializePointHp_.x + kTextureSizeHp_.x * num , kInitializePointHp_.y - num * kDistanceYHp_ };
	iterator.size = kTextureSizeHp_;
	UIManager::GetInstance().CreateSprite(iterator);
}

void Player::InitAudio() {
	//ダメージ
	hitSound_ = Audio::GetInstance().LoadWave("resource/Sound/damage.wav");
	//パリィ
	parrySound_ = Audio::GetInstance().LoadWave("resource/Sound/bane.wav");
}

void Player::ActionUpdate() {
	actionState_->Update(*this);
	actionState_->CommandInput(*this);

	if (actionState_->GetIsInput()) {
		ChangeStatePatternAction(actionState_->GetNextState());
	}
}


void Player::Update() {
	GameActor::Update();

	//弾丸更新処理
	BulletUpdate();

	//無敵時間
	InfinityTimeUpdate();

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

	CollisionManager::GetInstance().AddCollisions(this);
	collisionOverlap = CollisionManager::GetInstance().SetTarget(GetTranslate(), GetAABB());
	isGround_ = false;

}

void Player::BulletUpdate() {	
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
	else if (IsMovePosition() || !isGround_) {
		motionName_ = "move";
	}
	else {
		motionName_ = "standby";
	}

	if (isPerformance_) {
		motionName_ = "clear";
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
		particles_[particleWalk_.name]->SetParticleBorn(ParticleBorn::TimerMode);
		particles_[particleWalk_.name]->SetTranslate(transform_.translate + TransformNormal(Vector3{ 0.0f,-1.0f,-0.3f }, wt_.GetMatWorld()));
		particles_[particleWalk_.name]->SetScale({ 0.5f,0.5f,0.5f });
	}
	else {
		particles_[particleWalk_.name]->SetParticleBorn(ParticleBorn::Stop);
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
	object_->Update(wt_);

	wt_.UpdateMatrix(transform_);
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
	//発砲のクールタイム
	fireCoolTimer_ -= kDeltaTime_;
	fireCoolTimer_ = std::clamp(fireCoolTimer_, 0.0f, kFireCoolTimeMax_);

	//傘の方向を読み取る
	umbrellaRange_ = transformGun_.rotate;
	//円柱または円錐が縦のため、90度回転して横にする
	umbrellaRange_.x += kNinetyAngle_;

	//ノックバック発動
	KnockBackUpdate();

	//影の更新
	ShadowUpdate();

	//地面にいるとき
	if (isGround_) {
		jumpPower_ = 0.0f;//ジャンプ可能
		isOneBrink_ = false;//ブリンク可能
	}else {
		transform_.translate.y += jumpPower_;
	}

	//重力
	GravityUpdate(transform_.translate.y);
}

void Player::Gliding() {
	// - 滑空 - 
	//開いた状態で地面についていない
	//傘が上向き(斜め上も)の場合かつプレイヤーが倒されていないとき
	if (umbrella_->GetShieldMode() && !isGround_ &&
		(transformGun_.rotate.x >= kUpDis_ - kDiagonalValue_ && transformGun_.rotate.x <= kUpDis_ + kDiagonalValue_)) {
		GravityDown();
		//滑空中は上向きのみ(斜めにはならない)
		transformGun_.rotate.x = kUpDis_;
	}
}

void Player::GravityDown() {
	//ジャンプパワーリセット(ジャンプできない)
	jumpPower_ = 0.0f;
	//重力を固定することでゆっくり落ちる
	gravity_ = kFixedGravityPower_;
}


void Player::Active() {
	//プレイヤー操作
	//アクションステートの更新処理
	ActionUpdate();

	//生きている状態の更新処理
	LifeUpdate();

	//動ける範囲制限
	transform_.translate.x = std::clamp(transform_.translate.x, eventMin.x, eventMax.x);
	transform_.translate.y = std::clamp(transform_.translate.y, eventMin.y, eventMax.y);
	transform_.translate.z = std::clamp(transform_.translate.z, eventMin.z, eventMax.z);
	isEvent_ = false;

}

void Player::Dead() {
	//ノックバック、ダメージリアクション、ブリンクをリセット
	isKnockback_ = false;
	isDamageMotion_ = false;

	deadTimer_ += kDeltaTime_;
	isDead_ = true;

	//少しディレイを挟む(カメラのシェイクが終わったら)
	if (deadTimer_ >= kHitStopTime_) {
		//倒されたパーティクル配置+発動
		particles_[particleDead_.name]->SetTranslate(transform_.translate);
		particles_[particleDead_.name]->SetParticleBorn(ParticleBorn::TimerMode);

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
			particles_[particleDead_.name]->SetParticleBorn(ParticleBorn::Stop);
		}
	}
	else {
		//止まっているので発動しないようにする
		gravity_ = 0.0f;
		isGround_ = true;
	}
}

void Player::Performance() {}

#pragma region プレイヤーの操作

void Player::CommandMove() {
	//ゲームパット操作の場合
	if (Input::GetInstance().GetActiveGamePad()) {
		//Lスティック
		float padX = Input::GetInstance().LeftStickX();
		float padY = Input::GetInstance().LeftStickY();
		//左
		(padX > kStickPower_) ? isPushD_ = true : isPushD_ = false;
		//右
		(padX < -kStickPower_) ? isPushA_ = true : isPushA_ = false;
		//上
		(padY > kStickPower_) ? isPushW_ = true : isPushW_ = false;
		//下
		(padY < -kStickPower_) ? isPushS_ = true : isPushS_ = false;
	}
	else {
		//キーボード操作
		//左
		(Input::GetInstance().PushKey(DIK_A)) ? isPushA_ = true : isPushA_ = false;
		//右
		(Input::GetInstance().PushKey(DIK_D)) ? isPushD_ = true : isPushD_ = false;
		//上
		(Input::GetInstance().PushKey(DIK_W)) ? isPushW_ = true : isPushW_ = false;
		//下
		(Input::GetInstance().PushKey(DIK_S)) ? isPushS_ = true : isPushS_ = false;
	}
	//シールド中足が遅くなる
	//(滑空中は影響しない)
	if (umbrella_->GetShieldMode() && isGround_) {
		//スピードを半減させる
		const float gSlowSpeed = 0.5f;//半減する数値
		speed_ = kStandardSpeed_ * gSlowSpeed;
	}
	else {
		//元の速さ
		speed_ = kStandardSpeed_;
	}

	if (isPushA_) {
		transform_.translate.x -= speed_;//左に移動
		transform_.rotate.y = kDirectionLeft_;//左が正面に
		UmbrellaRange(kLeftDis_);//傘を左に
	}
	else if (isPushD_) {
		transform_.translate.x += speed_;//右に移動
		transform_.rotate.y = kDirectionRight_;//右が正面に
		UmbrellaRange(kRightDis_);//傘を右に
	}
	else if (isPushW_) {
		UmbrellaRange(kUpDis_);//傘を上に
	}
	else if (isPushS_) {
		UmbrellaRange(kDownDis_);//傘を下に
	}
}

void Player::CommandJump() {
	if (isGround_) {
		jumpPower_ = 0.3f;
	}
	isGround_ = false;
}

void Player::CommandFire() {
	//クールタイムは終了した時
	if (fireCoolTimer_ == 0.0f) {
		ShootBullet();
		fireCoolTimer_ = kFireCoolTimeMax_;
	}
}

void Player::CommandShield() {
	umbrella_->ShieldMode();
	Gliding();
}

void Player::CommandBrink() {
	//傘は開く
	umbrella_->ShieldMode();

	brinkTimer_ += kDeltaTime_;
	isOneBrink_ = true;//ブリンク一回目
	transform_.translate += EaseOut({ 0,0,0 }, TransformNormal({ 0,0,kBrinkPower_ }, wtGun_.GetMatWorld()), brinkTimer_ / kBrinkTimeMax_);

	//飛んだ瞬間後ろにパーティクルをだす
	if (brinkTimer_ <= kDeltaTime_) {
		Vector3 gTranslate = transform_.translate + TransformNormal(-kPlayerFront_, wtGun_.GetMatWorld());
		particles_[particleBrink_.name]->SetTranslate(gTranslate);
		particles_[particleBrink_.name]->SetRotate(umbrellaRange_);
		particles_[particleBrink_.name]->SetParticleBorn(ParticleBorn::MomentMode);
	}
	//地面についている場合、下向きのブリンクは発動しない
	if (isGround_ && (transformGun_.rotate.x > 0.0f && transformGun_.rotate.x < kLeftDis_)) {
		brinkTimer_ = kBrinkTimeMax_;
	}

	GravityDown();
}

bool Player::BrinkFlag() {
	if ((isPushA_ || isPushD_ || isPushW_ || isPushS_) && !isOneBrink_) {
		return true;
	}
	return false;
}

bool Player::BrinkTimeMax() {
	if (brinkTimer_ >= kBrinkTimeMax_) {
		brinkTimer_ = 0.0f; //タイマーリセット
		return true;
	}
	return false;
}

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

	if (hp_ != 0) {
		//傘
		umbrella_->Draw();
		//影
		shadow_->Draw();
	}
	
	Object3dCommon::GetInstance().Command();

	//弾丸
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
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

void Player::SetUmbrellaRotate() {
	//プレイヤーと同じY方向
	transformGun_.rotate.y = transform_.rotate.y;
}

void Player::ShootBullet() {	

	//傘から出るため
	Vector3 translate = umbrella_->GetTranslate();

	//真ん中を0にする値(3の場合、1,0,-1 | 5の場合、2,1,0,-1,-2)
	float halfCount = float((kBulletCount_ - 1) * kDivideByTwo_);//二で割る

	for (float i = -(halfCount); i <= halfCount; ++i) {
		//弾が分散するように
		Vector3 velocity = { 0.0f,i * kDispersionBetween_ ,kBulletSpeed_ };
		//飛ばす向きをwtGun_に合わせる
		velocity = TransformNormal(velocity, wtGun_.GetMatWorld());

		//弾丸を生み出す
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();
		bullet->SetTranslate(translate);//発泡初期位置
		bullet->SetVelocity(velocity);//速さ
		bullets_.push_back(std::move(bullet));
	}

	//攻撃パーティクル発生
	particles_[particleFire_.name]->SetTranslate(translate);
	particles_[particleFire_.name]->SetRotate(umbrellaRange_);
	particles_[particleFire_.name]->SetParticleBorn(ParticleBorn::MomentMode);

	///撃った方向と反対方向にノックバック
	KnockBackUmbrella(kBulletKnockbackPower_, kBulletSpeed_);
}

void Player::OffShield() {
	umbrella_->OffShield();
}


void Player::OnCollision(CollisionSource* collision) {
	if (collision->GetType() == CollisionTypes::enemyBullet || 
		collision->GetType() == CollisionTypes::bombExplotion || 
		collision->GetType() == CollisionTypes::boss) {
		IsDamage(GetDistance(collision->GetCenter()));
	}

	//演出中、死亡の時は当たらない
	if (collision->GetType() == CollisionTypes::stage
		&& hp_ != 0 && !isPerformance_) {
		CollisionManager::GetInstance().GameActorAndStageCollision(collisionOverlap,*this, *this,collision->GetAABB());
	}

	if (collision->GetType() == CollisionTypes::event) {
		//イベント範囲から出れないように
		eventMin = collision->GetAABB().min + transform_.scale;
		eventMax = collision->GetAABB().max - transform_.scale;
		isEvent_ = true;
	}

	if (!isEvent_) {
		//イベント範囲解放
		eventMin = -kMoveMax_;
		eventMax = kMoveMax_;
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
		particles_[particleDamage_.name]->SetTranslate(transform_.translate + Normalize(hitPoint));
		particles_[particleDamage_.name]->SetParticleBorn(ParticleBorn::MomentMode);
		//ダメージのSE再生
		Audio::GetInstance().SoundPlayWave(hitSound_, kVolume_);
		infinityTimer_ = 0.0f;//無敵時間発動
		//ノックバック(時間の三分の一ぶんまで)
		KnockBackPlayer(hitPoint , kInfinityTimeMax_ * kDivideByThree_);
	}
	//リアクションフラグ
	isDamageMotion_ = true;
}

void Player::IsFall() {
	if (hp_ == 0) {
		return;
	}
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
		isDead_ = false; //死亡フラグをなしに
		hp_ = maxHp_;    //体力を満タンに	
	}

	deadTimer_ = 0.0f;

	//移動範囲を元に戻す
	eventMin = -kMoveMax_;
	eventMax = kMoveMax_;
	isRespawn_ = true;

	transform_.translate = NextStageSave::GetInstance().GetNextStageSaveData().checkPoint;
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
	particles_[particleParry_.name]->SetTranslate(translate);
	particles_[particleParry_.name]->SetRotate(umbrellaRange_);
	particles_[particleParry_.name]->SetParticleBorn(ParticleBorn::MomentMode);
}

void Player::SpriteUpdate() {
	for (uint32_t i = 0; i < kPlayerMaxHp_; i++) {
		//Hpに応じてテクスチャを変化させる
		if (i >= hp_) {
			UIManager::GetInstance().SetSpriteTexture("playerHp" + std::to_string(i), "NoHp");
		}//テクスチャ体力ない状態なら変更
		else if (UIManager::GetInstance().GetSpriteTexture("playerHp" + std::to_string(i)) == "NoHp.png") {
			UIManager::GetInstance().SetSpriteTexture("playerHp" + std::to_string(i), "Hp");
		}
	}
}

void Player::UmbrellaRange(float direction) {
	//ブリンク中は角度を変更しない
	//上下左右
	transformGun_.rotate.x = direction;

	//斜めの時
	//左上と右下
	if ((isPushA_ && isPushW_) || (isPushD_ && isPushS_)) {
		transformGun_.rotate.x += kDiagonalValue_;
	}
	//左下と右上
	else if ((isPushA_ && isPushS_) || (isPushD_ && isPushW_)) {
		transformGun_.rotate.x -= kDiagonalValue_;
	}

	//360度を超えたらマイナスする
	if (transformGun_.rotate.x > kMaxAngle) {
		transformGun_.rotate.x -= kMaxAngle;
	}
}

const bool Player::IsMovePosition() {
	if (transform_.translate.x != prePosition_.x || transform_.translate.y != prePosition_.y) {
		return true;
	}
	return false;
}

void Player::ChangeStatePatternAction(std::unique_ptr<BasePlayerState> playerState) {
	actionState_.reset();
	actionState_ = std::move(playerState);
}