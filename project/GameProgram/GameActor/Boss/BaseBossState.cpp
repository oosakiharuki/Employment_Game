/// ------------------------------------
///
/// ボスの行動ステート一覧
/// 
/// ------------------------------------
#include "BaseBossState.h"
#include "Boss.h"
#include "UseEveryOne.h"

using namespace MyMath;
using namespace UseEveryOne;

bool BossCommand::FarMoveSuccess() {
	if (isFarMoveSuccess_) {
		isFarMoveSuccess_ = false;
		return true;
	}
	return false;
}



void BossMoveState::Update(BossCommand& bossCommand) {
	bossCommand.CommandMove();
}

void BossMoveState::ChangeCommand(Boss& boss) {
	if (boss.GetMotionFinish()) {
		boss.ChangeStatePattern(std::make_unique<BossBeforeActionMotionState>());
	}
}

void BossAttackState::Update(BossCommand& bossCommand) {
	//発泡処理
	bossCommand.CommandFire(kTimerFrame_, kBulletSpeed_, kRapidCountMax_);
}

void BossAttackState::ChangeCommand(Boss& boss) {
	if (boss.GetMotionFinish()) {
		boss.ChangeStatePattern(std::make_unique<BossMoveState>());
	}
}

void BossAroundMoveState::Update(BossCommand& bossCommand) {
	bossCommand.CommandAroundMove();
}

void BossAroundMoveState::ChangeCommand(Boss& boss) {
	if (boss.GetMotionFinish()) {
		boss.ChangeStatePattern(std::make_unique<BossMoveState>());
	}
}

void BossBeforeActionMotionState::Update(BossCommand& bossCommand) {
	bossCommand.CommandBeforeActionMotion();
}

void BossBeforeActionMotionState::ChangeCommand(Boss& boss) {
	//モーションが終了していないとき
	if (!boss.GetMotionFinish()) {
		return;
	}

	//移動カウント
	if (boss.ActionCount() >= kDeepAttackCountMax_) {
		boss.ResetActionCount();
		boss.ChangeStatePattern(std::make_unique<BossFarAttackState>());//3 last
	}
	else if (boss.ActionCount() >= kMoveCountMax_) {
		boss.AddActionCount();//行動カウント加算
		boss.ChangeStatePattern(std::make_unique<BossAroundMoveState>());//2
	}
	else {
		boss.AddActionCount();//行動カウント加算
		boss.ChangeStatePattern(std::make_unique<BossAttackState>());//1
	}		
}

void BossFarAttackState::Update(BossCommand& bossCommand) {
	if (bossCommand.FarMoveSuccess()) {
		isFire_ = true;
	}
	
	if (isFire_) {
		bossCommand.CommandFire(kTimerFrame_, kBulletSpeed_, kRapidCountMax_);
	}
	else {
		bossCommand.CommandFarMove();
	}
}

void BossFarAttackState::ChangeCommand(Boss& boss) {
	if (boss.GetMotionFinish()) {
		boss.ChangeStatePattern(std::make_unique<BossFarTackleState>());
	}
}


void BossFarTackleState::Update(BossCommand& bossCommand) {
	bossCommand.CommandFarTackle();
}

void BossFarTackleState::ChangeCommand(Boss& boss) {
	if (boss.GetMotionFinish()) {
		boss.ChangeStatePattern(std::make_unique<BossFallPlayerState>());
	}
}

void BossFallPlayerState::Update(BossCommand& bossCommand) {
	bossCommand.CommandFallPlayer();
}

void BossFallPlayerState::ChangeCommand(Boss& boss) {
	if (boss.GetMotionFinish()) {
		boss.ChangeStatePattern(std::make_unique<BossMoveState>());
	}
}
