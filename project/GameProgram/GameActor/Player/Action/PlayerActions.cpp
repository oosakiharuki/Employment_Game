#include "PlayerActions.h"
#include "Player.h"

#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void PlayerActions::InitAudio() {
	//発砲攻撃
	fireSound_ = Audio::GetInstance().LoadWave("resource/Sound/fire.mp3");
	//ジャンプ
	jumpSound_ = Audio::GetInstance().LoadWave("resource/Sound/jump.mp3");
}


void PlayerActions::BulletUpdate() {
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
}

void PlayerActions::BulletDraw() {
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void PlayerActions::JumpUpdate() {
	Vector3 translate = player_->GetTranslate();
	//ジャンプによる変動
	//地面にいるとき
	if (player_->GetIsGround()) {
		jumpPower_ = 0.0f;//ジャンプ可能
		isOneBrink_ = false;//ブリンク可能
	}
	else {
		translate.y += jumpPower_;
	}

	player_->SetTranslate(translate);
}

void PlayerActions::CommandMoveA() {
	MovePlayer(-speed_,kDirectionLeft_);//左に移動、左向きに
	UmbrellaRange(kLeftDis_);//傘を左に
}

void PlayerActions::CommandMoveD() {
	MovePlayer(speed_, kDirectionRight_);//右に移動、右向きに
	UmbrellaRange(kRightDis_);//傘を右に
}

void PlayerActions::CommandMoveW() {
	UmbrellaRange(kUpDis_);//傘を上に
}

void PlayerActions::CommandMoveS() {
	UmbrellaRange(kDownDis_);//傘を下に
}

void PlayerActions::SpeedParameter() {

	//標準ロック(移動しないで傘を動かすのみ)
	if ((Input::GetInstance().PushKey(DIK_I) || Input::GetInstance().PushButton(XINPUT_GAMEPAD_Y)) && player_->GetIsGround()) {
		speed_ = 0.0f; 
		prevDirectionWidth_ = 0.0f;
		return;
	}

	//シールド中足が遅くなる
	//(滑空中は影響しない)
	if (player_->GetUmbrellaShieldMode() && player_->GetIsGround()) {
		//スピードを半減させる
		const float gSlowSpeed = 0.5f;//半減する数値
		speed_ = kStandardSpeed_ * gSlowSpeed;
	}
	else {
		//元の速さ
		speed_ = kStandardSpeed_;
	}

	//記録リセット
	prevDirectionWidth_ = 0.0f;
}

void PlayerActions::MovePlayer(float speed, float playerDirection) {
	Vector3 translate = player_->GetTranslate();
	Vector3 rotate = player_->GetRotate();

	translate.x += speed;//左右どちらかに移動
	rotate.y = playerDirection;////左右どちらか正面に

	player_->SetTranslate(translate);
	player_->SetRotate(rotate);
}

void PlayerActions::UmbrellaRange(float direction) {
	Vector3 rotate = player_->GetUmbrellaRotate();
	//上下左右
	rotate.x = direction;

	//斜めの時
	//左上と右下
	if ((prevDirectionWidth_ == kLeftDis_ && direction == kUpDis_) || (prevDirectionWidth_ == kRightDis_ && direction == kDownDis_)) {
		rotate.x -= kDiagonalValue_;
	}
	//左下と右上
	else if ((prevDirectionWidth_ == kLeftDis_ && direction == kDownDis_) || (prevDirectionWidth_ == kRightDis_ && direction == kUpDis_)) {
		rotate.x += kDiagonalValue_;
	}

	//360度を超えたらマイナスする
	if (rotate.x > kMaxAngle) {
		rotate.x -= kMaxAngle;
	}
	player_->SetUmbrellaRotate(rotate);

	//横の向きを記録
	prevDirectionWidth_ = direction;
}


void PlayerActions::CommandJump() {
	if (player_->GetIsGround()) {
		jumpPower_ = kJumpPowerMax_;
		player_->ParticleJump();//ジャンプのパーティクル
		Audio::GetInstance().SoundPlayWave(*jumpSound_, kVolume_);
	}
	player_->IsGround(false);	
}

void PlayerActions::CommandFire() {
	//クールタイムは終了した時
	if (fireCoolTimer_ == 0.0f) {
		if (player_->UseGaugePoint()) {
			PowerShootBullet();//強い発砲攻撃
			player_->SubGaugePoint();//ゲージポイント減少
		}
		else {
			ShootBullet();//発砲攻撃
		}
		fireCoolTimer_ = kFireCoolTimeMax_;	
		
		Audio::GetInstance().StopWave(*fireSound_);
		Audio::GetInstance().SoundPlayWave(*fireSound_, kVolume_);
	}
}


void PlayerActions::ShootBullet() {
	BornBullet(player_->GetUmbrellaTranslate(), { 0.0f,kDispersionBetween_ ,kBulletSpeed_ });
}

void PlayerActions::PowerShootBullet() {
	//弾丸速度が二倍、より遠くに飛ばせる
	BornBullet(player_->GetUmbrellaTranslate(), { 0.0f,kDispersionBetween_ * kDivideByTwo_ ,kBulletSpeed_ * kTwice_ });
}

void PlayerActions::BornBullet(const Vector3& translate, const Vector3& velocity) {
	for (float i = -(halfCount); i <= halfCount; ++i) {	
		Vector3 bulletVelocity = velocity;
		//弾が分散するように
		bulletVelocity.y *= i;
		//飛ばす向きをwtGun_に合わせる
		bulletVelocity = TransformNormal(bulletVelocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();
		bullet->SetTranslate(translate);//発泡初期位置
		bullet->SetVelocity(bulletVelocity);//速さ
		bullets_.push_back(std::move(bullet));
	}
	//パーティクル
	player_->ParticleFire(translate);
	///撃った方向と反対方向にノックバック
	player_->KnockBackUmbrella(kBulletKnockbackPower_, kBulletSpeed_);
}

void PlayerActions::CommandShield() {
	//傘を開く
	player_->OnUmbrellaShield();
	Gliding();
}

void PlayerActions::CommandBrink() {		
	//地面についている場合、下向きのブリンクは発動しない、ゲージも使用しない
	if (player_->GetIsGround() && (player_->GetUmbrellaRotate().x > 0.0f && player_->GetUmbrellaRotate().x < kLeftDis_)) {
		brinkTimer_ = 0.0f;
		player_->StopParticleBrink();
		return;
	}

	//傘を開く
	player_->OnUmbrellaShield();

	//飛んだ瞬間
	if (brinkTimer_ == kBrinkTimeMax_) {
		player_->SubGaugePoint();//ゲージポイント減少
	}
	player_->ParticleBrink();

	brinkTimer_ -= kDeltaTime_;
	isOneBrink_ = true;//ブリンク一回目

	Vector3 translate = player_->GetTranslate();//プレイヤー座標を持ってくる
	//ブリンクの動き加算
	// brinkTimerがkBrinkTimeMaxを減算していくため (Max - (Maxから減算していく値))
	translate += EaseOut({ 0,0,0 }, TransformNormal({ 0,0,kBrinkPower_ }, player_->GetUmbrellaMatWorld()), (kBrinkTimeMax_ - brinkTimer_) / kBrinkTimeMax_);
	player_->SetTranslate(translate);//プレイヤー座標更新

	//時間が経過したら
	if (brinkTimer_ <= 0.0f) {
		OffShield();//シールド解除	
	}
	if (brinkTimer_ <= kBrinkTimeMax_ * kDivideByTwo_) {
		player_->StopParticleBrink();
	}
	jumpPower_ = 0.0f;//ジャンプによるの加算はされない
	player_->GravityDown();//重力加速度をなしに、
}

void PlayerActions::Gliding() {

	Vector3 rotate = player_->GetUmbrellaRotate();
	// - 滑空 - 
	//開いた状態で地面についていない
	//傘が上向き(斜め上も)の場合かつプレイヤーが倒されていないとき
	if (player_->GetUmbrellaShieldMode() && !player_->GetIsGround()) {
		jumpPower_ = 0.0f;
		player_->GravityDown();
		//滑空中は上向きのみ(斜めにはならない)
		rotate.x = kUpDis_;
		player_->SetUmbrellaRotate(rotate);
	}
}

bool PlayerActions::BrinkFlag() {
	if (!isOneBrink_ && player_->UseGaugePoint()) {
		brinkTimer_ = kBrinkTimeMax_;//タイマーを
		return true;
	}
	return false;
}

bool PlayerActions::BrinkTimeMax() {
	if (brinkTimer_ > 0.0f) {
		return true;
	}
	return false;
}

void PlayerActions::OffShield() {
	player_->OffUmbrellaShield();
}
