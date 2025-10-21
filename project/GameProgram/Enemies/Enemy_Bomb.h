#pragma once
#include "IEnemy.h"
#include "MyMath.h"

/// <summary>
/// ボムの敵(IEnemyの派生クラス)
/// </summary>
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
	//爆発開始
	bool isStart = false;

	//爆発するまでのタイマー
	const float bombTimeMax = 5.0f;//max
	float bombTimer = 0.0f;

	//敵のスピード
	Vector3 speed = { 0.03f,0.0f,0.0f };
	Vector3 move = { 0,0,0 };

	//パーティクル
	std::unique_ptr<Particle> particle_Bom;//爆発
	AABB bombAABB;
	Vector3 hani = { 3,3,3 };

	Vector3 distance;
};

