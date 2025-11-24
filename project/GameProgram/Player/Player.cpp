#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Primitive.h"

#include "SpriteCommon.h"
#include "SkinningCommon.h"
#include <numbers>

using namespace MyMath;

Player::Player(){}

Player::~Player() {}

void Player::Initialize() {

	Actor_InitializeCommon();

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
	wtGun_.rotation_.y = 90.0f;

	//SE
	//ダメージ
	hitSound_ = Audio::GetInstance()->LoadWave("resource/Sound/damage.wav");
	//パリィ
	pariSound_ = Audio::GetInstance()->LoadWave("resource/Sound/bane.wav");

	//パーティクル初期化
	//歩く
	particleWalk_ = std::make_unique<Particle>();
	particleWalk_->Initialize("player_walk", "resource/Sprite/ground.png", PrimitiveType::box);
	particleWalk_->SetParticleMosion(ParticleMosion::Smaller);
	//ブリンク
	particleBrink_ = std::make_unique<Particle>();
	particleBrink_->Initialize("player_brink", "resource/Sprite/cone.png", PrimitiveType::cone);
	particleBrink_->SetParticleCount(1);
	particleBrink_->SetParticleMosion(ParticleMosion::Fixed);
	particleBrink_->SetFrequency(1.0f);//時間
	particleBrink_->SetScale({2,2,2});
	//攻撃
	particleFire_ = std::make_unique<Particle>();
	particleFire_->Initialize("player_fire", "resource/Sprite/cone.png", PrimitiveType::cone);
	particleFire_->SetParticleCount(1);
	particleFire_->SetParticleMosion(ParticleMosion::Fixed);
	particleFire_->SetFrequency(0.1f);
	//ダメージ
	particleDamage_ = std::make_unique<Particle>();
	particleDamage_->Initialize("player_damage", "resource/Sprite/circle.png", PrimitiveType::ring);
	particleDamage_->SetParticleCount(20);
	particleDamage_->SetParticleMosion(ParticleMosion::Exprosion);
	particleDamage_->SetFrequency(1.0f);
	//パリィ
	particlePari_ = std::make_unique<Particle>();
	particlePari_->Initialize("player_pari", "resource/Sprite/uvChecker.png", PrimitiveType::cone);
	particlePari_->SetParticleMosion(ParticleMosion::Fixed);
	particleFire_->SetFrequency(0.5f);
	//死んだとき
	particleDead_ = std::make_unique<Particle>();
	particleDead_->Initialize("player_dead", "resource/Sprite/ground.png", PrimitiveType::sphere);
	particleDead_->SetParticleMosion(ParticleMosion::Smaller);
	particleDead_->SetFrequency(0.1f);
	particleDead_->SetScale({ 0.5f,0.5f,0.5f });

	//MaxHp初期設定
	maxHp_ = kPlayerMaxHp_;

	//UI_体力
	for (uint32_t i = 0; i < maxHp_; i++) {
		std::unique_ptr <Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize("Hp.png");
		sprite->SetPosition({ kInitializePointHp_.x + kTextureSizeHp_.x * i , kInitializePointHp_.y - i * kDistanceYHp_ });
		sprite->SetSize(kTextureSizeHp_);
		spritesHp_.push_back(std::move(sprite));
	}

	//入力処理
	input_ = Input::GetInstance();
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
		wt_.translation_.y += 0.25f;
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
		// 通常のパーティクル
		particleWalk_->SetParticleCount(5);
		particleWalk_->SetFrequency(0.15f);
		particleWalk_->SetParticleBorn(ParticleBorn::TimerMode);
		particleWalk_->SetTranslate(wt_.translation_ + TransformNormal(Vector3{ 0.0f,-1.0f,-0.3f }, wt_.matWorld_));
		particleWalk_->SetScale({ 0.5f,0.5f,0.5f });
	}
	else {
		particleWalk_->SetParticleBorn(ParticleBorn::Stop);
	}

	//パーティクル
	particleWalk_->Update();
	particleFire_->Update();
	particleBrink_->Update();
	particleDamage_->Update();
	particlePari_->Update();
	particleDead_->Update();

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

	ImGui::End();

#endif //  USE_IMGUI

	// - 更新 -

	wt_.UpdateMatrix();
	wtGun_.UpdateMatrix();

	// - 傘の銃 -
	//プレイヤーの手前に
	umbrella_->SetTranslate(wt_.translation_ +
		TransformNormal(playerFront_, wtGun_.matWorld_));
	//傘の方向
	umbrella_->SetRotate(wtGun_.rotation_);
	//防御状態か
	umbrella_->ShieldMode(isShield_);
	//更新
	umbrella_->Update();

	// - UI -
	//スプライト更新
	SpriteUpdate();
	for (auto& sprite : spritesHp_) {
		sprite->Update();
	}
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

		if (padX > 0.5f) {
			//左
			isPushD_ = true;
		}
		else if (padX < -0.5f) {
			//右
			isPushA_ = true;
		}

		if (padY > 0.5f) {
			//上
			isPushW_ = true;
		}
		else if (padY < -0.5f) {
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

	if (isBrink_) {
		//ブリンクの時は傘は開いたまま
		isShield_ = true;
		brinkTimer_ += kDeltaTime_;

		isOneBrink_ = true;
		wt_.translation_ += EaseOut(TransformNormal(playerFront_, wtGun_.matWorld_), brinkTimer_, kBrinkTimeMax_);

		//飛んだ瞬間後ろにパーティクルをだす
		if (brinkTimer_ <= kDeltaTime_) {
			Vector3 gTranslate = wt_.translation_ + TransformNormal(-playerFront_, wtGun_.matWorld_);
			particleBrink_->SetTranslate(gTranslate);
			particleBrink_->SetRotate({ wtGun_.rotation_.x + kNinetyAngle_,wtGun_.rotation_.y,wtGun_.rotation_.z });
			particleBrink_->SetParticleBorn(ParticleBorn::MomentMode);
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
		ScaleUpdate(&isDamageMosion_, damageScale_, kDamageMaxTime_);
	}
	//傘リアクション
	if (isShildMosion_) {
		umbrella_->ScaleUpdate(&isShildMosion_, damageScale_, kDamageMaxTime_);
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
	particleWalk_->Draw();
	particleFire_->Draw();
	particleBrink_->Draw();
	particleDamage_->Draw();
	particlePari_->Draw();
	particleDead_->Draw();

	SpriteCommon::GetInstance()->Command();
	//UI
	for (auto& sprite : spritesHp_) {
		sprite->Draw();
	}

}

void Player::ShootBullet() {	

	//傘から出るため
	Vector3 translate = umbrella_->GetTranslate();

	//真ん中を0にする値(3の場合、1,0,-1 | 5の場合、2,1,0,-1,-2)
	float halfCount = float((kBulletCount_ - 1) / 2);

	for (float i = -(halfCount); i <= halfCount; ++i) {
		//弾が分散するように
		Vector3 velocity = { 0.0f,float(i) * 0.1f,0.5f};
		velocity = TransformNormal(velocity, wtGun_.matWorld_);

		//弾丸を生み出す
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();
		bullet->SetTranslate(translate);
		bullet->SetVelocty(velocity);
		bullets_.push_back(std::move(bullet));
	}

	//攻撃パーティクル発生
	particleFire_->SetTranslate(translate);
	particleFire_->SetRotate({ wtGun_.rotation_.x + kNinetyAngle_,wtGun_.rotation_.y,wtGun_.rotation_.z });
	particleFire_->SetParticleBorn(ParticleBorn::MomentMode);

	///ノックバック
	Vector3 playerknockback = { 0.0f,0.0f,0.25f };
	KnockBackUmbrella(playerknockback,0.5f);
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
		particleDamage_->SetTranslate(wt_.translation_ + Normalize(hitPoint));
		particleDamage_->SetParticleBorn(ParticleBorn::MomentMode);
		//ダメージのSE再生
		Audio::GetInstance()->SoundPlayWave(hitSound_, kVolume_);
		infinityTimer_ = 0.0f;//無敵時間発動
		//ノックバック
		KnockBackPlayer(hitPoint , kInfinityTimeMax_ / 3);
	}

	isDamageMosion_ = true;
	
	//リアクション
	//連続ヒット時、大きさを元に戻す
	scaleTimer_ = 0.0f;
	wt_.scale_ = { 1,1,1 };

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

void Player::KnockBackPlayer(const Vector3 Power, const float TimerMax) {
	//威力を代入
	backPower_ = Normalize(Power);
	backPower_.z = 0.0f;//z方向はなし
	//ノックバックするフラグ
	isKnockback_ = true;
	//ノックバック時間(EaseOutで使用する)
	knockBackTimeMax_ = TimerMax;
}

void Player::KnockBackUmbrella(const Vector3 Power, const float TimerMax) {
	//威力を代入
	backPower_ = TransformNormal(Power, wtGun_.matWorld_);
	backPower_.z = 0.0f;//z方向はなし
	//ノックバックするフラグ
	isKnockback_ = true;
	//ノックバック時間(EaseOutで使用する)
	knockBackTimeMax_ = TimerMax;

	//連続ヒット時、元に戻す
	umbrella_->SetScale({ 1,1,1 });

}

void Player::DeadPlayer() {

	deadTimer_ += kDeltaTime_;
	isDead_ = true;
	
	//少しディレイを挟む(カメラのシェイクが終わったら)
	if (deadTimer_ >= kHitStopTime_) {
		
		//倒されたパーティクル配置+発動
		particleDead_->SetTranslate(wt_.translation_);
		particleDead_->SetParticleBorn(ParticleBorn::TimerMode);

		wt_.rotation_.y = 180.0f;//カメラのほうに向く
		wt_.rotation_.z += 10.0f;//回転する
		//少し浮く
		wt_.translation_.y += 0.3f;
		//重力
		GravityUpdate();

		isGround_ = false;
		if (deadTimer_ >= kDeadTimeMax_) {
			isRespawn_ = true;
			//パーティクル発動停止
			particleDead_->SetParticleBorn(ParticleBorn::Stop);
		}
	}
	else {
		//止まっているので発動しないようにする
		grabity_ = 0.0f;
		isGround_ = true;
	}
}

void Player::RespawnPlayer() {
	RespawnCommon();

	deadTimer_ = 0.0f;
	isRespawn_ = false;
}

//パリィ成功
void Player::PariSuccess() {
	//パリィ時間延長(連続弾でも跳ね返せるように)
	pariTime_ = kPariTimeMax_;

	Audio::GetInstance()->StopWave(pariSound_);
	Audio::GetInstance()->SoundPlayWave(pariSound_, kVolume_);

	Vector3 translate = umbrella_->GetTranslate();
	translate.x += TransformNormal({0,0,2}, wtGun_.matWorld_).x;

	particlePari_->SetTranslate(translate);
	particlePari_->SetRotate({ wtGun_.rotation_.x + kNinetyAngle_,wtGun_.rotation_.y,wtGun_.rotation_.z });
	particlePari_->SetScale({2.0f,0.2f,2.0f});

	particlePari_->SetParticleBorn(ParticleBorn::MomentMode);
}

void Player::SpriteUpdate() {
	float i = 0;
	for (auto& hp : spritesHp_) {
		//Hpに応じてテクスチャを変化させる
		if (i >= hp_) {
			hp->SetTextureFile("NoHp.png");
		}//テクスチャ体力ない状態なら変更
		else if (hp->GetTextureFile() == "NoHp.png") {
			hp->SetTextureFile("Hp.png");//体力ある状態
		}
		i++;
	}
}

void Player::UmbrellaRange(const float& direction) {
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