#pragma once
#include "IEnemy.h"
#include "EnemyBullet.h"

class Enemy_Turret : public IEnemy {
public:

	~Enemy_Turret() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Attack() override;

	void IsDamage() override;
	bool IsDead() { return isDead; }

	void ShootBullet();
	std::list<EnemyBullet*> GetBullets() { return bullets_; }

private:
	uint32_t maxHp = 30;
	uint32_t hp = maxHp;

	std::list<EnemyBullet*> bullets_;
	float coolTime = 0.0f;
};

