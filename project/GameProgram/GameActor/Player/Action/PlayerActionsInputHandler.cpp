/// --------------------------
///
/// プレイヤーの行動パターン
/// 
/// --------------------------
#include "PlayerActionsInputHandler.h"
#include "Player.h"
#include <Input.h>
#include "FoldingUmbrella.h"
#include <TimeScale.h>

using namespace MyMath;
using namespace UseEveryOne;

void MoveCommand::Execute() {
	//動く系の処理(スピードの調整)
	SpeedParameter();

	//Lスティック
	float padX = EngineLayer::Input::GetInstance().LeftStickX();
	float padY = EngineLayer::Input::GetInstance().LeftStickY();
	//左右
	if (padX > kStickPower_ || EngineLayer::Input::GetInstance().PushKey(DIK_D)) { CommandMoveD(); }
	else if (padX < -kStickPower_ || EngineLayer::Input::GetInstance().PushKey(DIK_A)) { CommandMoveA(); }
	//上下
	if (padY > kStickPower_ || EngineLayer::Input::GetInstance().PushKey(DIK_W)) { CommandMoveW(); }
	else if (padY < -kStickPower_ || EngineLayer::Input::GetInstance().PushKey(DIK_S)) { CommandMoveS(); }
}


void MoveCommand::CommandMoveA() {
	MovePlayer(-speed_, kDirectionLeft_);//左に移動、左向きに
	UmbrellaRange(kLeftDis_);//傘を左に
}

void MoveCommand::CommandMoveD() {
	MovePlayer(speed_, kDirectionRight_);//右に移動、右向きに
	UmbrellaRange(kRightDis_);//傘を右に
}

void MoveCommand::CommandMoveW() {
	UmbrellaRange(kUpDis_);//傘を上に
}

void MoveCommand::CommandMoveS() {
	UmbrellaRange(kDownDis_);//傘を下に
}

void MoveCommand::SpeedParameter() {

	//標準ロック(移動しないで傘を動かすのみ)
	if ((EngineLayer::Input::GetInstance().PushKey(DIK_I) || EngineLayer::Input::GetInstance().PushButton(XINPUT_GAMEPAD_Y)) && player_->GetIsGround()) {
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

void MoveCommand::MovePlayer(float speed, float playerDirection) {
	Vector3 translate = player_->GetTranslate();
	Vector3 rotate = player_->GetRotate();

	translate.x += speed;//左右どちらかに移動
	rotate.y = playerDirection;////左右どちらか正面に

	player_->SetTranslate(translate);
	player_->SetRotate(rotate);
}


void MoveCommand::UmbrellaRange(float direction) {
	Vector3 rotate = player_->GetUmbrellaRotate();
	//上,下,左,右のどれか
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
	if (rotate.x > kMaxAngle_) {
		rotate.x -= kMaxAngle_;
	}
	player_->SetUmbrellaRotate(rotate);

	//横の向きを記録
	prevDirectionWidth_ = direction;
}


void JumpCommand::Execute() {
	//ジャンプ処理
	if (player_->GetIsGround()) {
		player_->SetJumpPower(kJumpPowerMax_);
		player_->ParticleJump();//ジャンプのパーティクル
	}
	player_->IsGround(false);
}


void FireCommand::Execute() {
	//発泡処理(傘から発砲)
	player_->FireBulletUmbrella();
}

void ShieldCommand::Execute() {
	//シールド処理
	//傘を開く
	player_->OnUmbrellaShield();
	Gliding();
}

void ShieldCommand::Gliding() {

	Vector3 rotate = player_->GetUmbrellaRotate();
	// - 滑空 - 
	//開いた状態で地面についていない
	//傘が上向き(斜め上も)の場合かつプレイヤーが倒されていないとき
	if (player_->GetUmbrellaShieldMode() && !player_->GetIsGround()) {
		player_->SetJumpPower(0.0f);
		player_->GravityDown();
		//滑空中は上向きのみ(斜めにはならない)
		rotate.x = kUpDis_;
		player_->SetUmbrellaRotate(rotate);
	}
}

void BrinkCommand::Execute() {
	//ブリンク処理
	float brinkTimer = player_->GetBrinkTimer();

	//地面についている場合、下向きのブリンクは発動しない、ゲージも使用しない
	if (player_->GetIsGround() && (player_->GetUmbrellaRotate().x > 0.0f && player_->GetUmbrellaRotate().x < kLeftDis_)) {
		brinkTimer = 0.0f;
		player_->StopParticleBrink();
	}

	//飛んだ瞬間
	if (brinkTimer == kBrinkTimeMax_) {
		player_->SubGaugePoint();//ゲージポイント減少
	}

	//ブリンクのパーティクルを出す
	player_->ParticleBrink();

	brinkTimer -= TimeScale::GetInstance().GetTimeScale();
	player_->IsOneBrink();//ブリンク一回目

	Vector3 translate = player_->GetTranslate();//プレイヤー座標を持ってくる
	//ブリンクの動き加算
	// brinkTimerがkBrinkTimeMaxを減算していくため (Max - (Maxから減算していく値))
	translate += EaseOut({ 0,0,0 }, TransformNormal({ 0,0,kBrinkPower_ }, player_->GetUmbrellaMatWorld()), (kBrinkTimeMax_ - brinkTimer) / kBrinkTimeMax_) * TimeScale::GetInstance().GetTimeScaleFacto();
	player_->SetTranslate(translate);//プレイヤー座標更新

	//時間が経過したら
	if (brinkTimer <= 0.0f) {
		player_->OffUmbrellaShield();//シールド解除	
	}
	if (brinkTimer <= kBrinkTimeMax_ * kDivideByTwo_) {
		player_->StopParticleBrink();
	}
	player_->SetJumpPower(0.0f);//ジャンプによるの加算はされない
	player_->GravityDown();//重力加速度をなしに、

	player_->SetBrinkTimer(brinkTimer);
}

void WeaponChangeCommand::Execute() {
	//銃の番号
	weaponNum_ = player_->GetWeaponNum();

	if (EngineLayer::Input::GetInstance().TriggerKey(DIK_U) || EngineLayer::Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		RightSwitching();//;
	}
	if (EngineLayer::Input::GetInstance().TriggerKey(DIK_O) || EngineLayer::Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		LeftSwitching();//;
	}

	if (weaponNum_ != player_->GetWeaponNum()) {
		auto u = weaponUmbrella_.find(weaponNum_);
		player_->WeaponChangeUmbrella(std::move(u->second()), weaponNum_);
		prevWeaponNum_ = weaponNum_;
	}
}

void WeaponChangeCommand::RightSwitching() {
	weaponNum_++;
	if (weaponNum_ > kWeaponMax_) {
		weaponNum_ = 0;
	}
}

void WeaponChangeCommand::LeftSwitching() {
	weaponNum_--;
	if (weaponNum_ < 0) {
		weaponNum_ = kWeaponMax_;
	}
}


void PlayerActionsInputHandler::GetCommand(Player* player, std::vector<std::unique_ptr<BaseCommand>>& commands) {

#ifdef USE_IMGUI

	if (EngineLayer::Input::GetInstance().TriggerKey(DIK_F4)) {
		isInput_ = !isInput_;
	}

	if (isInput_) {
		return;
	}

#endif // USE_IMGUI

	if (player->IsFiring()) {
		commands.push_back(std::make_unique<FireCommand>());
		//移動
		commands.push_back(std::make_unique<MoveCommand>());
		return;
	}

	//ブリンク [Jキー、Bボタン + ブリンクの発動条件、発動中はタイマーが切れるまで]
	if (((EngineLayer::Input::GetInstance().TriggerKey(DIK_J) || EngineLayer::Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_B)) && player->BrinkFlag()) || player->BrinkTimeMax()) {
		commands.push_back(std::make_unique<BrinkCommand>());
		return;
	}	
	
	//移動
	commands.push_back(std::make_unique<MoveCommand>());

	//傘シールド / 滑空[L/Jキー、Lトリガー、Bボタンを長押し] (ブリンク発動後でも作動できるようにボタンを統一)
	if (EngineLayer::Input::GetInstance().PushKey(DIK_L) || EngineLayer::Input::GetInstance().LeftTriggerLongPress() ||
		EngineLayer::Input::GetInstance().PushKey(DIK_J) || EngineLayer::Input::GetInstance().PushButton(XINPUT_GAMEPAD_B)) {
		commands.push_back(std::make_unique<ShieldCommand>());
		return;
	}
	else {
		//傘シールド状態解除
		player->OffUmbrellaShield();
	}

	//ジャンプ[spaceキー、Aボタン]
	if (EngineLayer::Input::GetInstance().TriggerKey(DIK_SPACE) || EngineLayer::Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) {
		commands.push_back(std::make_unique<JumpCommand>());
	}
	
	//発砲攻撃[Kキー、Xボタン、Rトリガー]
	if (EngineLayer::Input::GetInstance().TriggerKey(DIK_K) || EngineLayer::Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_X) || EngineLayer::Input::GetInstance().RightTrigger()) {
		commands.push_back(std::make_unique<FireCommand>());
	}

	commands.push_back(std::make_unique<WeaponChangeCommand>());
}

