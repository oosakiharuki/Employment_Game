#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Primitive.h"

#include "SpriteCommon.h"
#include "SkinningCommon.h"
#include <numbers>

#include "ParticleManager.h"

using namespace MyMath;
using namespace UseEveryOne;

Player::Player(){}

Player::~Player() {}

void Player::Initialize() {

	Actor_InitializeCommon();

	//アニメーションを保管
	objectMosions_["standby"] = "player_standby.gltf";
	objectMosions_["move"] = "player_move.gltf";
	objectMosions_["shield"] = "player_shield.gltf";
	objectMosions_["clear"] = "player_clear.gltf";

	//プレイヤー初期化/オブジェクト読み込み
	object_ = std::make_unique<Object_glTF>();
	object_->Initialize();
	object_->SetModelFile(objectMosions_[mosionName_]);

	//傘の初期化
	umbrella_ = std::make_unique<Umbrella>();
	umbrella_->Initialize();

	//傘の行列
	wtGun_.Initialize();

	//SE
	//ダメージ
	hitSound_ = Audio::GetInstance()->LoadWave("resource/Sound/damage.wav");
	//パリィ
	pariSound_ = Audio::GetInstance()->LoadWave("resource/Sound/bane.wav");

	//パーティクル初期化
	particles_[particleWalk_.name] = ParticleManager::GetInstance()->InitParticle(particleWalk_);
	particles_[particleBrink_.name] = ParticleManager::GetInstance()->InitParticle(particleBrink_);
	particles_[particleFire_.name] = ParticleManager::GetInstance()->InitParticle(particleFire_);
	particles_[particleDamage_.name] = ParticleManager::GetInstance()->InitParticle(particleDamage_);
	particles_[particlePari_.name] = ParticleManager::GetInstance()->InitParticle(particlePari_);
	particles_[particleDead_.name] = ParticleManager::GetInstance()->InitParticle(particleDead_);

	//MaxHp初期設定
	maxHp_ = kPlayerMaxHp_;

	//UI_体力
	for (uint32_t i = 0; i < maxHp_; i++) {
		SpriteData iterator;
		iterator.name = "playerHp" + std::to_string(i);
		iterator.texturePath = "Hp";
		iterator.position = { kInitializePointHp_.x + kTextureSizeHp_.x * i , kInitializePointHp_.y - i * kDistanceYHp_ };
		iterator.size = kTextureSizeHp_;
		UI::GetInstance()->CreateSprite(iterator);
	}

	//入力処理
	input_ = Input::GetInstance().get();
}

void Player::Update() {

	//倒された時
	if (hp_ == 0) {
		//ノックバック、ダメージリアクション、ブリンクをリセット
		isKnockback_ = false;
		isDamageMosion_ = false;
		isBrink_ = false;
		DeadPlayer();
	}

	if (!isDead_ && !isPerformance_) {
		//プレイヤー操作/アクション
		PlayUpdate();
	}

	//地面にいるとき
	if (isGround_) {
		isJump_ = false;//ジャンプ可能
		isOneBrink_ = false;//ブリンク可能
	}

	// - 滑空 - 
	//開いた状態で地面についていない
	//傘が上向き(斜め上も)の場合かつプレイヤーが倒されていないとき
	if (isShield_ && !isGround_ && 
		(wtGun_.rotation_.x >= kUpDis_ - kNanameValue_ && wtGun_.rotation_.x <= kUpDis_ + kNanameValue_) && !isDead_) {
		//ジャンプ後だとずっと浮くためfalseに
		if (isJump_) {
			isJump_ = false;
		}
		//重力を固定することでゆっくり落ちる
		grabity_ = kFixedGrabityPower_;
		//ブリンクが終了した時
		if (!isBrink_) {
			//滑空中は上向きのみ(斜めにはならない)
			wtGun_.rotation_.x = kUpDis_;
		}
	}

	//ジャンプ
	if (isJump_) {
		wt_.translation_.y += kJumpUp_;
	}

	//演出時は関係なし
	if (!isPerformance_ && !isDead_) {
		//重力
		GravityUpdate();
	}

	//弾丸
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

	//無敵時間
	if (infinityTimer_ >= kInfinityTimeMax_) {
		infinityTimer_ = kInfinityTimeMax_;//Maxになったら無敵時間終了
	}
	else {
		infinityTimer_ += kDeltaTime_;
	}

	//移動しているとパーティクルを発生
	if (isGround_ && IsMovePosition()) {
		// 歩く煙パーティクル
		particles_[particleWalk_.name]->SetParticleBorn(ParticleBorn::TimerMode);
		particles_[particleWalk_.name]->SetTranslate(wt_.translation_ + TransformNormal(Vector3{ 0.0f,-1.0f,-0.3f }, wt_.matWorld_));
		particles_[particleWalk_.name]->SetScale({ 0.5f,0.5f,0.5f });
	}
	else {
		particles_[particleWalk_.name]->SetParticleBorn(ParticleBorn::Stop);
	}

	//パーティクル
	for (auto& p : particles_) {
		p.second->Update();
	}

	///アニメーション
	if (isShield_) {
		mosionName_ = "shield";
	}//前回の座標と現在の座標が違う = 動いた場合
	else if (IsMovePosition()) {
		mosionName_ = "move";
	}
	else {
		mosionName_ = "standby";
	}

	if (isPerformance_) {
		mosionName_ = "clear";
	}

	//animationが変わった場合切り替える
	if (mosionName_ != preMosionName_) {
		object_->ChangeAnimation(objectMosions_[mosionName_]);
		preMosionName_ = mosionName_;
	}

	//現在座標に前回座標を代入
	prePosition_ = wt_.translation_;
	
	object_->Update(wt_);

#ifdef  USE_IMGUI

	ImGui::Begin("player");

	ImGui::InputFloat3("worldTransform.translate", &wt_.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &wt_.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("Rotate", &wt_.rotation_.x);
	ImGui::SliderFloat("RotateX", &wt_.rotation_.x, -kMaxAngle, kMaxAngle);
	ImGui::SliderFloat("RotateY", &wt_.rotation_.y, -kMaxAngle, kMaxAngle);
	ImGui::SliderFloat("RotateZ", &wt_.rotation_.z, -kMaxAngle, kMaxAngle);

	ImGui::Text("体力:%d", hp_);
	ImGui::Text("残機 %d", zanki_);

	ImGui::End();

#endif //  USE_IMGUI

	// - 更新 -

	wt_.UpdateMatrix();
	wtGun_.UpdateMatrix();

	// - 傘の銃 -
	//プレイヤーの手前に
	umbrella_->SetTranslate(wt_.translation_ +
		TransformNormal(kPlayerFront_, wtGun_.matWorld_));
	//傘の方向
	umbrella_->SetRotate(wtGun_.rotation_);
	//防御状態か
	umbrella_->ShieldMode(isShield_);
	//更新
	umbrella_->Update();

	// - UI -
	//スプライト更新
	SpriteUpdate();
}

void Player::PlayUpdate() {

	input_->JoystickUpdate(state_, preState_);

	//それぞれのボタンフラグ
	isPushA_ = false;
	isPushD_ = false;
	isPushW_ = false;
	isPushS_ = false;

	//左
	if (input_->PushKey(DIK_A)) {
		isPushA_ = true;
	}
	//右
	if (input_->PushKey(DIK_D)) {
		isPushD_ = true;
	}
	//上
	if (input_->PushKey(DIK_W)) {
		isPushW_ = true;
	}
	//下
	if (input_->PushKey(DIK_S)) {
		isPushS_ = true;
	}

	//Lスティック
	if (input_->GetJoystickState(0, state_)) {
		float padX = static_cast<float>(state_.Gamepad.sThumbLX) / 32768.0f;
		float padY = static_cast<float>(state_.Gamepad.sThumbLY) / 32768.0f;

		if (padX > kStickPower_) {
			//左
			isPushD_ = true;
		}
		else if (padX < -kStickPower_) {
			//右
			isPushA_ = true;
		}

		if (padY > kStickPower_) {
			//上
			isPushW_ = true;
		}
		else if (padY < -kStickPower_) {
			//下
			isPushS_ = true;
		}
	}

	//シールド中足が遅くなる
	//(滑空中は影響しない)
	if (isShield_ && isGround_) {
		//スピードを半減させる
		const float gSlowSpeed = 0.5f;//半減する数値
		speed_ = kStandardSpeed_ * gSlowSpeed;
	}
	else {
		//元の速さ
		speed_ = kStandardSpeed_;
	}

	if (isPushA_) {
		wt_.translation_.x -= speed_;//左に移動
		wt_.rotation_.y = kDirectionLeft_;//左が正面に
		UmbrellaRange(kLeftDis_);//傘を左に
	}
	else if (isPushD_) {
		wt_.translation_.x += speed_;//右に移動
		wt_.rotation_.y = kDirectionRight_;//右が正面に
		UmbrellaRange(kRightDis_);//傘を右に
	}
	else if (isPushW_) {
		UmbrellaRange(kUpDis_);//傘を上に
	}
	else if (isPushS_) {
		UmbrellaRange(kDownDis_);//傘を下に
	}

	//ジャンプ
	//指定したボタン、地面についていて傘がシールド状態でないとき
	if ((input_->TriggerKey(DIK_SPACE) || input_->TriggerBotton(state_, preState_, XINPUT_GAMEPAD_A))
		&& isGround_ && !isShield_) {
		isJump_ = true;
		isGround_ = false;
	}

	//傘シールド
	if (input_->PushKey(DIK_L) || input_->PushBotton(state_, XINPUT_GAMEPAD_B)) {
		//押した瞬間に移動キーを押している場合 + すでにブリンクを一度している場合
		if ((input_->TriggerKey(DIK_L) || input_->TriggerBotton(state_, preState_, XINPUT_GAMEPAD_B))
			&& (isPushA_ || isPushD_ || isPushW_ || isPushS_) && !isOneBrink_) {
			//ブリンクが発動
			isBrink_ = true;
		}
		isShield_ = true;

		pariTime_ -= kDeltaTime_;
		//パリィ時間がすぎるとき+ダメージを食らていたらパリィできない
		if (pariTime_ > 0.0f && infinityTimer_ >= kInfinityTimeMax_) {
			isPari_ = true;
		}
		else {
			isPari_ = false;
		}

		//パリィ時間リセット
		pariCoolTime_ = 0.0f;
	}
	else {
		isShield_ = false;
		pariCoolTime_ += kDeltaTime_;
	}
	//連打してもすぐにパリィできないようにする
	if (pariCoolTime_ >= kPariTimeMax_) {
		pariTime_ = kPariTimeMax_;
	}

	//傘の方向を読み取る
	umbrellaRange_ = wtGun_.rotation_;
	//円柱または円錐が縦のため、90度回転して横にする
	umbrellaRange_.x += kNinetyAngle_;

	if (isBrink_) {
		//ブリンクの時は傘は開いたまま
		isShield_ = true;
		brinkTimer_ += kDeltaTime_;

		isOneBrink_ = true;
		wt_.translation_ += EaseOut(TransformNormal(kPlayerFront_, wtGun_.matWorld_), brinkTimer_, kBrinkTimeMax_);

		//飛んだ瞬間後ろにパーティクルをだす
		if (brinkTimer_ <= kDeltaTime_) {
			Vector3 gTranslate = wt_.translation_ + TransformNormal(-kPlayerFront_, wtGun_.matWorld_);
			particles_[particleBrink_.name]->SetTranslate(gTranslate);
			particles_[particleBrink_.name]->SetRotate(umbrellaRange_);
			particles_[particleBrink_.name]->SetParticleBorn(ParticleBorn::MomentMode);
		}
	}
	//地面についている場合、下向きのブリンクは発動しない
	if (isGround_ && (wtGun_.rotation_.x > 0.0f && wtGun_.rotation_.x < kLeftDis_)) {
		brinkTimer_ = kBrinkTimeMax_;
	}
	if (brinkTimer_ >= kBrinkTimeMax_) {
		isBrink_ = false;
		brinkTimer_ = 0.0f;
	}

	coolTimer_ += kDeltaTime_;
	if ((input_->TriggerKey(DIK_K) || input_->TriggerBotton(state_, preState_, XINPUT_GAMEPAD_X)) && !isShield_) {
		if (coolTimer_ >= kCoolTimeMax_) {
			ShootBullet();
			coolTimer_ = 0;
		}
	}

	//ダメージリアクション
	if (isDamageMosion_) {
		reaction_->ScaleReaction(wt_.scale_, isDamageMosion_, damageScale_,scaleTimer_, kDamageMaxTime_);
	}
	//傘リアクション
	if (isShildMosion_) {
		umbrella_->HitReaction(isShildMosion_);
	}

	//ノックバック発動
	if (isKnockback_) {
		//ゼロならイーズインされない
		if (knockBackTimeMax_ == 0.0f) {
			wt_.translation_ -= backPower_;
			isKnockback_ = false;
		}
		else {
			knockBackTimer_ += kDeltaTime_;

			wt_.translation_ -= EaseOut(backPower_, knockBackTimer_, knockBackTimeMax_);
			if (knockBackTimer_ >= knockBackTimeMax_) {
				isKnockback_ = false;
				knockBackTimer_ = 0.0f;
			}
		}
	}

	//影の更新
	shadow_->SetTranslate(wt_.translation_);
}

void Player::Draw() {
	GLTFCommon::GetInstance()->Command();
	//プレイヤー本体
	object_->Draw();

	if (hp_ != 0) {
		//傘
		umbrella_->Draw();
		//影
		shadow_->Draw();
	}
	
	Object3dCommon::GetInstance()->Command();

	//弾丸
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}

}

void Player::DrawP() {
	//パーティクル
	for (auto& particle : particles_) {
		particle.second->Draw();
	}
}


Vector3 Player::GetWorldPosition() const {
	Vector3 worldPos;

	worldPos.x = wt_.matWorld_.m[3][0];
	worldPos.y = wt_.matWorld_.m[3][1];
	worldPos.z = wt_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetUmbrellaRotate() {
	//プレイヤーと同じY方向
	wtGun_.rotation_.y = wt_.rotation_.y;
}

void Player::ShootBullet() {	

	//傘から出るため
	Vector3 translate = umbrella_->GetTranslate();

	//真ん中を0にする値(3の場合、1,0,-1 | 5の場合、2,1,0,-1,-2)
	float halfCount = float((kBulletCount_ - 1) * kDivideByTwo_);//二で割る

	for (float i = -(halfCount); i <= halfCount; ++i) {
		//弾が分散するように
		Vector3 velocity = { 0.0f,float(i) * kDisparsionBetween_ ,kBulletSpeed_ };
		//飛ばす向きをwtGun_に合わせる
		velocity = TransformNormal(velocity, wtGun_.matWorld_);

		//弾丸を生み出す
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();
		bullet->SetTranslate(translate);//発泡初期位置
		bullet->SetVelocty(velocity);//速さ
		bullets_.push_back(std::move(bullet));
	}

	//攻撃パーティクル発生
	particles_[particleFire_.name]->SetTranslate(translate);
	particles_[particleFire_.name]->SetRotate(umbrellaRange_);
	particles_[particleFire_.name]->SetParticleBorn(ParticleBorn::MomentMode);

	///撃った方向と反対方向にノックバック
	KnockBackUmbrella(kBulletKnockbackPower_, kBulletSpeed_);
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
		particles_[particleDamage_.name]->SetTranslate(wt_.translation_ + Normalize(hitPoint));
		particles_[particleDamage_.name]->SetParticleBorn(ParticleBorn::MomentMode);
		//ダメージのSE再生
		Audio::GetInstance()->SoundPlayWave(hitSound_, kVolume_);
		infinityTimer_ = 0.0f;//無敵時間発動
		//ノックバック(時間の三分の一ぶんまで)
		KnockBackPlayer(hitPoint , kInfinityTimeMax_ * kDivideByThree_);
	}
	//リアクションフラグ
	isDamageMosion_ = true;
}

void Player::IsFall() {
	if (hp_ == 0) {
		return;
	}
	//一発K.O
	hp_ = 0;
	//ダメージSE再生
	Audio::GetInstance()->SoundPlayWave(hitSound_, kVolume_);
}

void Player::KnockBackPlayer(const Vector3& Power, float TimerMax) {
	//威力を代入
	backPower_ = Normalize(Power);
	KnockBackCommon(TimerMax);	
	//リアクション
	//連続ヒット時、大きさを元に戻す
	scaleTimer_ = 0.0f;
	wt_.scale_ = kDefaultScale_;
}

void Player::KnockBackUmbrella(const Vector3& Power, float TimerMax) {
	//威力を代入(傘の向きに沿って)
	backPower_ = TransformNormal(Power, wtGun_.matWorld_);
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

void Player::IsShildMosion() {
	isShildMosion_ = true;
	umbrella_->ResetScaleTimer();
	umbrella_->SetScale(kDefaultScale_);
}

void Player::DeadPlayer() {

	deadTimer_ += kDeltaTime_;
	isDead_ = true;
	
	//少しディレイを挟む(カメラのシェイクが終わったら)
	if (deadTimer_ >= kHitStopTime_) {
		
		//倒されたパーティクル配置+発動
		particles_[particleDead_.name]->SetTranslate(wt_.translation_);
		particles_[particleDead_.name]->SetParticleBorn(ParticleBorn::TimerMode);

		DirectionTheCamera();//カメラのほうに向く
		wt_.rotation_.z += kPlayerDeadRotating_;//回転する
		//少し浮く
		wt_.translation_.y += kDeadLittleUp_;
		//重力
		GravityUpdate();

		isGround_ = false;
		if (deadTimer_ >= kDeadTimeMax_) {
			isRespawn_ = true;
			//パーティクル発動停止
			particles_[particleDead_.name]->SetParticleBorn(ParticleBorn::Stop);
		}
	}
	else {
		//止まっているので発動しないようにする
		grabity_ = 0.0f;
		isGround_ = true;
	}
}

void Player::RespawnPlayer() {
	if (zanki_ == 0) {
		return;
	}
	//残機を減らす
	zanki_--;

	RespawnCommon();

	deadTimer_ = 0.0f;
	isRespawn_ = false;
}

//パリィ成功
void Player::PariSuccess() {
	//パリィ時間延長(連続弾でも跳ね返せるように)
	pariTime_ = kPariTimeMax_;

	//SE
	Audio::GetInstance()->StopWave(pariSound_);//パリィが続くとき一度止めてから再生させるようにする
	Audio::GetInstance()->SoundPlayWave(pariSound_, kVolume_);//SE再生:パリィ
	//傘の座標を読み取る
	Vector3 translate = umbrella_->GetTranslate();
	translate += TransformNormal(kPlayerFront_, wtGun_.matWorld_);//出す場所をwtGun_の向きの前に
	particles_[particlePari_.name]->SetTranslate(translate);
	particles_[particlePari_.name]->SetRotate(umbrellaRange_);
	particles_[particlePari_.name]->SetParticleBorn(ParticleBorn::MomentMode);
}

void Player::SpriteUpdate() {

	for (uint32_t i = 0; i < kPlayerMaxHp_; i++) {
		//Hpに応じてテクスチャを変化させる
		if (i >= hp_) {
			UI::GetInstance()->SetSpriteTexture("playerHp" + std::to_string(i), "NoHp");
		}//テクスチャ体力ない状態なら変更
		else if (UI::GetInstance()->GetSpriteTexture("playerHp" + std::to_string(i)) == "NoHp.png") {
			UI::GetInstance()->SetSpriteTexture("playerHp" + std::to_string(i), "Hp");
		}
	}
}

void Player::UmbrellaRange(float direction) {
	//ブリンク中は角度を変更しない
	if (!isBrink_) {
		//上下左右
		wtGun_.rotation_.x = direction;

		//斜めの時
		//左上と右下
		if ((isPushA_ && isPushW_) || (isPushD_ && isPushS_)) {
			wtGun_.rotation_.x += kNanameValue_;
		}
		//左下と右上
		else if ((isPushA_ && isPushS_) || (isPushD_ && isPushW_)) {
			wtGun_.rotation_.x -= kNanameValue_;
		}
	}

	//360度を超えたらマイナスする
	if (wtGun_.rotation_.x > kMaxAngle) {
		wtGun_.rotation_.x -= kMaxAngle;
	}
}

void Player::GravityUpdate() {
	//重力
	grabity_ -= kGrabityPower_;
	wt_.translation_.y += grabity_;
}

const bool Player::IsMovePosition() {
	if (wt_.translation_.x != prePosition_.x || wt_.translation_.y != prePosition_.y) {
		return true;
	}
	return false;
}