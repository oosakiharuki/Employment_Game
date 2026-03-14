#include "BasePlayerState.h"
#include "Player.h"

#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

void PlayerCommand::BulletUpdate() {
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

void PlayerCommand::BulletDraw() {
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}
}

void PlayerCommand::JumpUpdate() {
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









void PlayerCommand::CommandMove() {
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
	if (player_->GetUmbrellaShieldMode() && player_->GetIsGround()) {
		//スピードを半減させる
		const float gSlowSpeed = 0.5f;//半減する数値
		speed_ = kStandardSpeed_ * gSlowSpeed;
	}
	else {
		//元の速さ
		speed_ = kStandardSpeed_;
	}

	Vector3 translate = player_->GetTranslate();
	Vector3 rotate = player_->GetRotate();
	if (isPushA_) {
		translate.x -= speed_;//左に移動
		rotate.y = kDirectionLeft_;//左が正面に
		UmbrellaRange(kLeftDis_);//傘を左に
	}
	else if (isPushD_) {
		translate.x += speed_;//右に移動
		rotate.y = kDirectionRight_;//右が正面に
		UmbrellaRange(kRightDis_);//傘を右に
	}
	else if (isPushW_) {
		UmbrellaRange(kUpDis_);//傘を上に
	}
	else if (isPushS_) {
		UmbrellaRange(kDownDis_);//傘を下に
	}

	player_->SetTranslate(translate);
	player_->SetRotate(rotate);
}

void PlayerCommand::CommandJump() {
	if (player_->GetIsGround()) {
		jumpPower_ = kJumpPowerMax_;
	}
	player_->IsGround(false);
}

void PlayerCommand::CommandFire() {
	//クールタイムは終了した時
	if (fireCoolTimer_ == 0.0f) {
		ShootBullet();
		fireCoolTimer_ = kFireCoolTimeMax_;
	}
}


void PlayerCommand::ShootBullet() {

	//傘から出るため
	Vector3 translate = player_->GetUmbrellaTranslate();

	//真ん中を0にする値(3の場合、1,0,-1 | 5の場合、2,1,0,-1,-2)
	float halfCount = float((kBulletCount_ - 1) * kDivideByTwo_);//二で割る

	for (float i = -(halfCount); i <= halfCount; ++i) {
		//弾が分散するように
		Vector3 velocity = { 0.0f,i * kDispersionBetween_ ,kBulletSpeed_ };
		//飛ばす向きをwtGun_に合わせる
		velocity = TransformNormal(velocity, player_->GetUmbrellaMatWorld());

		//弾丸を生み出す
		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();
		bullet->SetTranslate(translate);//発泡初期位置
		bullet->SetVelocity(velocity);//速さ
		bullets_.push_back(std::move(bullet));
	}

	player_->ParticleFire(translate);


	///撃った方向と反対方向にノックバック
	player_->KnockBackUmbrella(kBulletKnockbackPower_, kBulletSpeed_);
}


void PlayerCommand::UmbrellaRange(float direction) {

	Vector3 rotate = player_->GetUmbrellaRotate();
	//ブリンク中は角度を変更しない
	//上下左右
	rotate.x = direction;

	//斜めの時
	//左上と右下
	if ((isPushA_ && isPushW_) || (isPushD_ && isPushS_)) {
		rotate.x += kDiagonalValue_;
	}
	//左下と右上
	else if ((isPushA_ && isPushS_) || (isPushD_ && isPushW_)) {
		rotate.x -= kDiagonalValue_;
	}

	//360度を超えたらマイナスする
	if (rotate.x > kMaxAngle) {
		rotate.x -= kMaxAngle;
	}

	player_->SetUmbrellaRotate(rotate);
}

void PlayerCommand::CommandShield() {
	//傘を開く
	player_->OnUmbrellaShield();
	Gliding();
}

void PlayerCommand::CommandBrink() {
	//傘を開く
	player_->OnUmbrellaShield();

	brinkTimer_ += kDeltaTime_;
	isOneBrink_ = true;//ブリンク一回目
	Vector3 translate = player_->GetTranslate();

	translate += EaseOut({ 0,0,0 }, TransformNormal({ 0,0,kBrinkPower_ }, player_->GetUmbrellaMatWorld()), brinkTimer_ / kBrinkTimeMax_);

	player_->SetTranslate(translate);

	//飛んだ瞬間後ろにパーティクルをだす
	if (brinkTimer_ <= kDeltaTime_) {
		player_->ParticleBrink();
	}
	//地面についている場合、下向きのブリンクは発動しない
	if (player_->GetIsGround() && (player_->GetUmbrellaRotate().x > 0.0f && player_->GetUmbrellaRotate().x < kLeftDis_)) {
		brinkTimer_ = kBrinkTimeMax_;
	}

	jumpPower_ = 0.0f;
	player_->GravityDown();
}



void PlayerCommand::Gliding() {

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

bool PlayerCommand::BrinkFlag() {
	if ((isPushA_ || isPushD_ || isPushW_ || isPushS_) && !isOneBrink_) {
		return true;
	}
	return false;
}

bool PlayerCommand::BrinkTimeMax() {
	if (brinkTimer_ >= kBrinkTimeMax_) {
		brinkTimer_ = 0.0f; //タイマーリセット
		return true;
	}
	return false;
}

void PlayerCommand::OffShield() {
	player_->OffUmbrellaShield();
}
















void PlayerNormalState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandMove();//移動
}

void PlayerNormalState::CommandInput(PlayerCommand& command) {
	if (Input::GetInstance().TriggerKey(DIK_SPACE) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) {
		nextState_ = std::make_unique<PlayerJumpState>();
	}
	if (Input::GetInstance().TriggerKey(DIK_K) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_X) || Input::GetInstance().RightTrigger()) {
		nextState_ = std::make_unique<PlayerFireState>();
	}
	if (Input::GetInstance().TriggerKey(DIK_L) || Input::GetInstance().LeftTrigger()) {
		//ブリンクの条件を満たしているか
		nextState_ = std::make_unique<PlayerShieldState>();
	}
	if (command.BrinkFlag() && (Input::GetInstance().TriggerKey(DIK_J) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_B))) {
		nextState_ = std::make_unique<PlayerBrinkState>(); 
	}
}

void PlayerJumpState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandMove();//移動
	playerCommand.CommandJump();//ジャンプ
}

void PlayerJumpState::CommandInput(PlayerCommand& command) {
	//すぐにステートを変更
	nextState_ = std::make_unique<PlayerNormalState>();
}

void PlayerFireState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandMove();//移動
	playerCommand.CommandFire();//発砲攻撃
}
void PlayerFireState::CommandInput(PlayerCommand& command) {
	//すぐにステートを変更
	nextState_ = std::make_unique<PlayerNormalState>();
}

void PlayerShieldState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandMove();//移動
	playerCommand.CommandShield();//傘で守る
}

void PlayerShieldState::CommandInput(PlayerCommand& command) {
	//ボタンが離れたとき、ステートを変更(シールド、ブリンクで使用しているボタン)
	if (!Input::GetInstance().PushKey(DIK_L) && !Input::GetInstance().PushKey(DIK_J) && !Input::GetInstance().PushButton(XINPUT_GAMEPAD_B) && !Input::GetInstance().LeftTriggerLongPress()) {
		command.OffShield();//シールドフラグオフ
		nextState_ = std::make_unique<PlayerNormalState>();
	}
}

void PlayerBrinkState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandBrink();//ブリンク(ここでは移動処理は行わない)
}

void PlayerBrinkState::CommandInput(PlayerCommand& command) {
	//ブリンクが最大まで行ったとき、ステートを変更
	if (command.BrinkTimeMax()) {
		nextState_ = std::make_unique<PlayerShieldState>();
	}
}