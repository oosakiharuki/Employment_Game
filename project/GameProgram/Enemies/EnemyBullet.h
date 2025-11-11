#pragma once
#include "Object3d.h"
#include "MyMath.h"

class Player;

/// <summary>
/// 敵の弾丸
/// </summary>
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
	void IsHit();
	AABB GetAABB();

	void Pari_Mode();
	bool GetIsPari() { return isPari; }

	Vector3 GetDistance() { return distance; }
	void SetPlayer(Player* player) { player_ = player; }

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

	Vector3 distance;

	//プレイヤークラス
	Player* player_ = nullptr;
};

