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

	bool IsDead() { return isDead; }
	bool IsExplosion() { return isExplosion; }

	void RespawnEnemy() override;
	
	/// <summary>
	/// getter‗ワールド座標系の座標
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition();

	/// <summary>
	/// getter‗爆発範囲AABB
	/// </summary>
	/// <returns></returns>bombAABB
	AABB GetBombAABB() { return bombAABB; }

	/// <summary>
	/// getter_距離
	/// </summary>
	/// <returns></returns>プレイヤーからボムの距離
	Vector3 GetDistance() { return distance; }

private:
	//爆発開始
	bool isStart = false;
	//爆発
	bool isExplosion = false;

	//爆発するまでのタイマー
	const float bombTimeMax = 5.0f;//max
	float bombTimer = 0.0f;

	//爆発範囲AABB
	AABB bombAABB;
	const Vector3 hani = { 10,10,10 };//大きさ
	//プレイヤーとの距離感
	Vector3 distance;

	//リアクション
	Vector3 bombScale = { 0.05f, 0.05f, 0.05f };


	/// <summary>
	/// 爆発する
	/// </summary>
	void Exprosion();

	void TimeRimmit();
};

