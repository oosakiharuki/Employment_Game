#include "BasePlayerState.h"
#include "Player.h"

#include "UseEveryOne.h"

using namespace UseEveryOne;

void PlayerNormalState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandMove();//移動
}

void PlayerNormalState::CommandInput(Player& player) {
	if (Input::GetInstance().TriggerKey(DIK_SPACE) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) {
		nextState_ = std::make_unique<PlayerJumpState>();
	}
	if (Input::GetInstance().TriggerKey(DIK_K) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_X) || Input::GetInstance().RightTrigger()) {
		nextState_ = std::make_unique<PlayerFireState>();
	}
	if (Input::GetInstance().TriggerKey(DIK_L) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_B) || Input::GetInstance().LeftTrigger()) {
		//ブリンクの条件を満たしているか
		if (player.BrinkFlag()) {
			nextState_ = std::make_unique<PlayerBrinkState>(); 
		}
		else {
			nextState_ = std::make_unique<PlayerShieldState>();
		}
	}
}

void PlayerJumpState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandMove();//移動
	playerCommand.CommandJump();//ジャンプ
}

void PlayerJumpState::CommandInput(Player& player) {
	//すぐにステートを変更
	nextState_ = std::make_unique<PlayerNormalState>();
}

void PlayerFireState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandMove();//移動
	playerCommand.CommandFire();//発砲攻撃
}
void PlayerFireState::CommandInput(Player& player) {
	//すぐにステートを変更
	nextState_ = std::make_unique<PlayerNormalState>();
}

void PlayerShieldState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandMove();//移動
	playerCommand.CommandShield();//傘で守る
}

void PlayerShieldState::CommandInput(Player& player) {
	//ボタンが離れたとき、ステートを変更
	if (!Input::GetInstance().PushKey(DIK_L) && !Input::GetInstance().PushButton(XINPUT_GAMEPAD_B) && !Input::GetInstance().LeftTriggerLongPress()) {
		player.OffShield();//シールドフラグオフ
		nextState_ = std::make_unique<PlayerNormalState>();
	}
}

void PlayerBrinkState::Update(PlayerCommand& playerCommand) {
	playerCommand.CommandBrink();//ブリンク(ここでは移動処理は行わない)
}

void PlayerBrinkState::CommandInput(Player& player) {
	//ブリンクが最大まで行ったとき、ステートを変更
	if (player.BrinkTimeMax()) {
		nextState_ = std::make_unique<PlayerShieldState>();
	}
}