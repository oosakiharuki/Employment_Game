#pragma once
#include "IEnemy.h"
#include "MyMath.h"

class Enemy_Bomb : public IEnemy {
public:
	~Enemy_Bomb() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Attack() override;

	void IsDamage() override;
	bool IsDead() { return isDead; }

	void RespownEnemy() override;

	Vector3 GetWorldPosition();
	void Exprosion();

	AABB GetBombAABB() { return bombAABB; }

	Vector3 GetDistance() { return distance; }

private:

	bool isStart = false;

	const float bombTimeMax = 5.0f;
	float bombTimer = 0.0f;

	const float deltaTime = 1.0f / 60.0f;

	Vector3 speed = { 0.03f,0.0f,0.0f };
	Vector3 move = { 0,0,0 };

	Particle* particle_Bom;
	AABB bombAABB;
	Vector3 hani = { 3,3,3 };

	Vector3 distance;
};

