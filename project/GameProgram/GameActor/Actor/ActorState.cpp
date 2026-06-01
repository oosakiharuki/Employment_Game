/// -------------------------------------------
///
/// アクターの状態
/// (生存.active、死亡.dead、演出.performance)
/// 
/// -------------------------------------------
#include "ActorState.h"
#include "GameActor.h"

#include "UseEveryOne.h"

using namespace UseEveryOne;

void ActiveState::Update(GameActor& actor) {
	actor.Active();
}

void ActiveState::ChangeState(GameActor& actor) {
	//演出中なら
	if (actor.GetPerformanceMode()) {
		//PerformanceStateに変更
		actor.ChangeStatePattern(std::make_unique<PerformanceState>());
	}
	else if (actor.GetHp() == 0) {
		//DeadStateに変更
		actor.ChangeStatePattern(std::make_unique<DeadState>());
	}
}


void DeadState::Update(GameActor& actor) {
	actor.Dead();
}

void DeadState::ChangeState(GameActor& actor) {
	if (!actor.GetHp() == 0) {
		//復活した(死んでいない)場合LifeStateに変更		
		actor.ChangeStatePattern(std::make_unique<ActiveState>());
	}
}

void PerformanceState::Update(GameActor& actor) {
	actor.Performance();
}

void PerformanceState::ChangeState(GameActor& actor) {
	if (!actor.GetPerformanceMode()) {
		actor.ChangeStatePattern(std::make_unique<ActiveState>());
	}
}