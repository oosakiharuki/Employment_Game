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
	
	void TimeRimmit();

	bool IsDead() { return isDead; }

	void RespownEnemy() override;
	
	/// <summary>
	/// getter‗ワールド座標系の座標
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition();
	/// <summary>
	/// 爆発する
	/// </summary>
	void Exprosion();

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
	//爆発終了
	bool isEnd = false;

	//爆発するまでのタイマー
	const float bombTimeMax = 5.0f;//max
	float bombTimer = 0.0f;

	//敵のスピード
	Vector3 speed = { 0.03f,0.0f,0.0f };
	Vector3 move = { 0,0,0 };

	//爆発範囲AABB
	AABB bombAABB;
	Vector3 hani = { 3,3,3 };//大きさ
	//プレイヤーとの距離感
	Vector3 distance;
};

