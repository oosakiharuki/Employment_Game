#include "BaseBossState.h"
#include "Boss.h"

void BaseBossState::Update(Boss& boss) {}

void BossMoveState::Update(Boss& boss) {

	boss.ResetMoveSucces();

	//偶数か奇数か
	if (std::fmod(boss.MoveCount(), 2) == 0) {
		boss.MoveRight();
	}
	else {
		boss.MoveLeft();
	}

	if (boss.IsMoveSucces()) {
		boss.AddMoveCount();
		boss.ChangeStatePattern(std::make_unique<BossAttackState>());	
	}
}

void BossAttackState::Update(Boss& boss) {
	boss.ResetStopFire();
	boss.Fire();

	if (boss.IsStopFire()) {
		boss.ChangeStatePattern(std::make_unique<BossMoveState>());
	}
}

