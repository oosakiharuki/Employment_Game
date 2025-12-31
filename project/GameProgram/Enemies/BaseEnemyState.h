#pragma once

class BaseEnemy;

class BaseEnemyState {
public:
	virtual void Update(BaseEnemy& enemy);
	virtual void StateLog();
protected:
	void EnemyDead(BaseEnemy& enemy);
};

class EnemyMoveState : public BaseEnemyState {
public:
	void Update(BaseEnemy& enemy) override;
	void StateLog() override;
};

class EnemyAttackState : public BaseEnemyState {
public:
	void Update(BaseEnemy& enemy) override;
	void StateLog() override;
};

class EnemyDeadState : public BaseEnemyState {
public:
	void Update(BaseEnemy& enemy) override;
	void StateLog() override;
};