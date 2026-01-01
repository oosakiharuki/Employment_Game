#pragma once
#include <cstdint>

class Boss;

class BaseBossState {
public:
	virtual void Update(Boss& boss);
};

class BossMoveState : public BaseBossState {
public:
	void Update(Boss& boss) override;
};

class BossAttackState : public BaseBossState {
public:
	void Update(Boss& boss) override;
};
