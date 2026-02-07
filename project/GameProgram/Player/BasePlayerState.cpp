#include "BasePlayerState.h"
#include "Player.h"

#include "UseEveryOne.h"

using namespace UseEveryOne;

void PlayerLifeState::Update(Player& player) {

	//アクションステートの更新処理
	player.ActionUpdate();

	//プレイヤー操作/アクション
	player.PlayUpdate();
	//生きている状態の更新処理
	player.LifeUpdate();

	//演出中なら
	if (player.GetPerformanceMode()) {
		//PerformanceStateに変更
		player.ChangeStatePattern(std::make_unique<PlayerPerformanceState>());
	}
	else if (player.GetHp() == 0) {
		//DeadStateに変更
		player.ChangeStatePattern(std::make_unique<PlayerDeadState>());
	}
}

void PlayerDeadState::Update(Player& player) {

	if (!player.GetIsDead() && player.GetHp() > 0) {
		//復活した(死んでいない)場合LifeStateに変更
		player.ChangeStatePattern(std::make_unique<PlayerLifeState>());
	}
	else {
		//倒された時
		player.DeadPlayer();
	}
}

void PlayerPerformanceState::Update(Player& player) {
	if (!player.GetPerformanceMode()) {
		player.ChangeStatePattern(std::make_unique<PlayerLifeState>());
	}
}


void PlayerNormalState::Update(Player& player) {
	player.ActionMove();

	if ((Input::GetInstance().TriggerKey(DIK_K) || 
		Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_X) ||
		Input::GetInstance().RightTrigger()) && !player.GetIsShield()) {
		player.ChangeStatePattern(std::make_unique<PlayerFireState>());
		return;
	}

	if ((Input::GetInstance().TriggerKey(DIK_L) || 
		Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_B) || 
		Input::GetInstance().LeftTrigger()) && !player.GetIsShield()) {
		player.ChangeStatePattern(std::make_unique<PlayerShieldState>());
		return;
	}

	//指定したボタン、地面についていて傘がシールド状態でないとき
	if ((Input::GetInstance().TriggerKey(DIK_SPACE) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) && player.GetIsGround()) {
		player.ChangeStatePattern(std::make_unique<PlayerJumpState>());
		return;
	}
}

void PlayerJumpState::Update(Player& player) {
	player.ActionJump();
	player.ActionMove();
	player.ChangeStatePattern(std::make_unique<PlayerNormalState>());
}

void PlayerFireState::Update(Player& player) {
	player.ActionFire();
	player.ActionMove();
	player.ChangeStatePattern(std::make_unique<PlayerNormalState>());
}

void PlayerShieldState::Update(Player& player) {
	//ブリンクの条件+瞬間のみ()
	if (player.BrinkFlag() && !player.GetIsShield()) {
		player.ChangeStatePattern(std::make_unique<PlayerBrinkState>());
		return;
	}

	player.ActionShield();
	player.ActionMove();
	player.Gliding();//滑空

	//押していなければ
	if (!Input::GetInstance().PushKey(DIK_L) && !Input::GetInstance().PushButton(XINPUT_GAMEPAD_B) && !Input::GetInstance().LeftTriggerLongPress()) {
		player.SetIsShield(false);
		player.ChangeStatePattern(std::make_unique<PlayerNormalState>());
	}
}

void PlayerBrinkState::Update(Player& player) {
	player.ActionShield();
	player.ActionBrink(brinkTimer_, kBrinkTimeMax_);
	player.GravityDown();//滑空と同じ重力(弱める)

	//押していなければ
	if (brinkTimer_ >= kBrinkTimeMax_) {
		player.ChangeStatePattern(std::make_unique<PlayerShieldState>());
	}
}
