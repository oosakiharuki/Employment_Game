#include "PlayerActionsInputHandler.h"
#include "PlayerActions.h"
#include <Input.h>

void MoveCommand::Execute(PlayerActions* player) {
	//動く系の処理(スピードの調整)
	player->SpeedParameter();

	//Lスティック
	float padX = Input::GetInstance().LeftStickX();
	float padY = Input::GetInstance().LeftStickY();
	//左右
	if (padX > kStickPower_ || Input::GetInstance().PushKey(DIK_D)) { player->CommandMoveD(); }
	else if (padX < -kStickPower_ || Input::GetInstance().PushKey(DIK_A)) { player->CommandMoveA(); }
	//上下
	if (padY > kStickPower_ || Input::GetInstance().PushKey(DIK_W)) { player->CommandMoveW(); }
	else if (padY < -kStickPower_ || Input::GetInstance().PushKey(DIK_S)) { player->CommandMoveS(); }
}
void JumpCommand::Execute(PlayerActions* player) {
	//移動も同時に行えるように
	moveCommand_ = std::make_unique<MoveCommand>();
	moveCommand_->Execute(player);
	//ジャンプ処理
	player->CommandJump();
}

void FireCommand::Execute(PlayerActions* player) {
	//移動も同時に行えるように
	moveCommand_ = std::make_unique<MoveCommand>();
	moveCommand_->Execute(player);
	//発泡処理
	player->CommandFire();
}

void ShieldCommand::Execute(PlayerActions* player) {
	//移動も同時に行えるように
	moveCommand_ = std::make_unique<MoveCommand>();
	moveCommand_->Execute(player);
	//シールド処理
	player->CommandShield();
}

void BrinkCommand::Execute(PlayerActions* player) {
	//ブリンク処理
	player->CommandBrink();
}
std::unique_ptr<Command> PlayerActionsInputHandler::GetCommand() {
	//ブリンク [Jキー、Bボタン + ブリンクの発動条件、発動中はタイマーが切れるまで]
	if (((Input::GetInstance().TriggerKey(DIK_J) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_B)) && playerActions_->BrinkFlag()) || playerActions_->BrinkTimeMax()) {
		return std::make_unique<BrinkCommand>();
	}//傘シールド / 滑空[L/Jキー、Lトリガー、Bボタンを長押し] (ブリンク発動後でも作動できるようにボタンを統一)
	else if (Input::GetInstance().PushKey(DIK_L) || Input::GetInstance().LeftTriggerLongPress() || 
		Input::GetInstance().PushKey(DIK_J) || Input::GetInstance().PushButton(XINPUT_GAMEPAD_B)) {
		return std::make_unique<ShieldCommand>();
	}

	//傘シールド状態解除
	playerActions_->OffShield();

	//ジャンプ[spaceキー、Aボタン]
	if (Input::GetInstance().TriggerKey(DIK_SPACE) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_A)) {
		return std::make_unique<JumpCommand>();
	}//発砲攻撃[Kキー、Xボタン、Rトリガー]
	else if (Input::GetInstance().TriggerKey(DIK_K) || Input::GetInstance().TriggerButton(XINPUT_GAMEPAD_X) || Input::GetInstance().RightTrigger()) {
		return std::make_unique<FireCommand>();
	}

	//移動のみ
	return std::make_unique<MoveCommand>();
}

