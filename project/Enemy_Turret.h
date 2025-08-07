#pragma once
#include "IEnemy.h"

class Enemy_Turret : public IEnemy {
public:

	~Enemy_Turret() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Attack() override;

	void IsDamage() override;
	bool IsDead() { return isDead; }

	void Fire();

private:
	uint32_t maxHp = 30;
	uint32_t hp = maxHp;

	float coolTime = 0.0f;

	float rapidFireTime = 0.0f;

	uint32_t rapidCount = 0;
	const uint32_t rapidFireMax = 5;
};

