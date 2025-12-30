#pragma once

class IEnemy;

class BaseEnemyState {
public:
	virtual void Update(IEnemy& enemy);
	virtual void StateLog();
protected:
	void EnemyDead(IEnemy& enemy);
};

class EnemyMoveState : public BaseEnemyState {
public:
	void Update(IEnemy& enemy) override;
	void StateLog() override;
};

class EnemyAttackState : public BaseEnemyState {
public:
	void Update(IEnemy& enemy) override;
	void StateLog() override;
};

class EnemyDeadState : public BaseEnemyState {
public:
	void Update(IEnemy& enemy) override;
	void StateLog() override;
};