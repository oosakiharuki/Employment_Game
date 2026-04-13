#include "BaseEnemyState.h"
#include "BaseEnemy.h"

void EnemySearchState::Update(BaseEnemy& enemy) {
	enemy.SearchCommand();
}

void EnemySearchState::ChangeState(BaseEnemy& enemy) {
	//見つかったら
	if (enemy.IsFoundTarget()) {
		enemy.AttackFlag();
		enemy.ChangeStatePattern(std::make_unique<EnemyAttackState>());
	}
}

void EnemyAttackState::Update(BaseEnemy& enemy) {
	enemy.AttackCommand();
}
void EnemyAttackState::ChangeState(BaseEnemy& enemy) {
	//見失ったら
	if (enemy.IsLostFound()) {
		enemy.ChangeStatePattern(std::make_unique<EnemySearchState>());
	}
}