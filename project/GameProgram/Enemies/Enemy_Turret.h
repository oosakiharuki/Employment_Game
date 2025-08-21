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

	void RespownEnemy() override;

private:
	bool isBulletStart = false;

	float coolTime = 0.0f;
	const float coolTimeMax = 1.0f;

	float rapidFireTime = 0.0f;
	const float rapidFireTimeMax = 0.1f;

	uint32_t rapidCount = 0;
	const uint32_t rapidFireMax = 6;

	Particle* particle_dead;
	Particle* particle_damage;
	Particle* particle_fire;
};

