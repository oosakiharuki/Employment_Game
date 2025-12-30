#include "BaseEnemyState.h"
#include "IEnemy.h"

void BaseEnemyState::Update(IEnemy& enemy) {}
void BaseEnemyState::StateLog() {}

void BaseEnemyState::EnemyDead(IEnemy& enemy) {
	if (enemy.GetHp() == 0) {
		enemy.ChangeStatePattern(std::make_unique<EnemyDeadState>());
	}
}

void EnemyMoveState::Update(IEnemy& enemy) {
	//重力
	enemy.GrabityUpdate();
	//プレイヤーの発見
	enemy.PlayerTarget();

	//角度
	enemy.DirectionDegree();

	//通常状態の更新処理
	enemy.UpdateNormal();


	//見つかったら
	if (enemy.IsFoundTarget()) {
		enemy.BulletFlag();
		enemy.ChangeStatePattern(std::make_unique<EnemyAttackState>());
	}

	//hp = 0
	EnemyDead(enemy);
}
void EnemyMoveState::StateLog() {
	Logger::log("EnemyState : move");
}

void EnemyAttackState::Update(IEnemy& enemy) {
	//重力
	enemy.GrabityUpdate();
	//プレイヤーの発見
	enemy.PlayerTarget();

	//攻撃フェーズの処理
	enemy.UpdateAttack();

	//!,?のマークの更新
	enemy.MarkUpdate();

	//見失ったら
	if (enemy.IsLostFound()) {
		enemy.ChangeStatePattern(std::make_unique<EnemyMoveState>());
	}

	//hp = 0
	EnemyDead(enemy);
}
void EnemyAttackState::StateLog() {
	Logger::log("EnemyState : attack");
}

void EnemyDeadState::Update(IEnemy& enemy) {
	//死んだときの処理
	enemy.UpdateDead();
}
void EnemyDeadState::StateLog() {
	Logger::log("EnemyState : dead");
}