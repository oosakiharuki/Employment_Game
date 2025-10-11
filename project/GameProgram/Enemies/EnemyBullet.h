#pragma once
#include "Object3d.h"
#include "MyMath.h"

class EnemyBullet {
public:

	~EnemyBullet();
	void Initialize();
	void Update();
	void Draw();

	Vector3 GetTranslate() { return wt.translation_; }
	void SetTranslate(Vector3 translate) { wt.translation_ = translate; }
	void SetVelocty(Vector3 velocity) { velocity_ = velocity; }

	bool IsDead() { return isDead; }
	void IsHit() { isDead = true; }
	AABB GetAABB();

	void Pari_Mode();
	bool GetIsPari() { return isPari; }

private:
	std::unique_ptr<Object3d> object = nullptr;
	WorldTransform wt;
	Vector3 velocity_;

	float deathTimer = 0.0f;
	const float endTime = 2.0f;
	bool isDead = false;

	AABB bulletAABB;

	//パリィされたら敵のほうにダメージになる
	bool isPari = false;
};

