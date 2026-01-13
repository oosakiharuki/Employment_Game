#include "BasePlayerState.h"
#include "Player.h"

void BasePlayerState::Update(Player& player) {}

void PlayerLifeState::Update(Player& player) {

	//演出中なら
	if (player.GetPerformanceMode()) {
		//PerformanceStateに変更
		player.ChangeStatePattern(std::make_unique<PlayerPerformanceState>());
	}
	else if (player.GetHp() == 0) {
		//DeadStateに変更
		player.ChangeStatePattern(std::make_unique<PlayerDeadState>());
	}
	else {
		//プレイヤー操作/アクション
		player.PlayUpdate();
		//生きている状態の更新処理
		player.LifeUpdate();
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