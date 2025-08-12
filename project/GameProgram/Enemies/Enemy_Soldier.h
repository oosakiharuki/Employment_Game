#pragma once
#include "IEnemy.h"

class Enemy_Soldier : public IEnemy{
public:
	~Enemy_Soldier() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Attack() override;

	void IsDamage() override;
	bool IsDead() { return isDead; }

	void Fire();


private:

	float coolTime = 0.0f;
	const float coolTimeMax = 1.0f;

	float rapidFireTime = 0.0f;
	const float rapidFireTimeMax = 0.1f;

	uint32_t rapidCount = 0;
	const uint32_t rapidFireMax = 3;
	Vector3 velocity;

	
	float rotateTimer = 0.0f;
	const float rotateTimeMax = 3.0f;

};