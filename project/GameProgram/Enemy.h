#pragma once
#include "IEnemy.h"

class Enemy : public IEnemy{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Attack() override;

	void IsDamage() override;
	bool IsDead() { return isDead; }

private:
	uint32_t maxHp = 33;
	uint32_t hp = maxHp;
};