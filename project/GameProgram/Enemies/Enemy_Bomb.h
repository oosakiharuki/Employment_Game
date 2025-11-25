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
	void DrawParticle() override;
	void Attack() override;
	bool IsExplosion()override { return isExplosion_; }
	void ExplosionEnd()override { isExplosion_ = true; }
	void RespawnEnemy() override;

	/// <summary>
	/// getter‗爆発範囲AABB
	/// </summary>
	/// <returns></returns>bombAABB
	AABB GetBombAABB()override { return bombAABB_; }

	/// <summary>
	/// getter_距離
	/// </summary>
	/// <returns></returns>プレイヤーからボムの距離
	Vector3 GetDistance()override { return distance_; }

private:
	//爆発開始
	bool isStart_ = false;
	//爆発
	bool isExplosion_ = false;

	//爆発するまでのタイマー
	const float kBombTimeMax_ = 5.0f;//max
	float bombTimer_ = 0.0f;

	//爆発範囲AABB
	AABB bombAABB_;
	const Vector3 kExplosionRange_ = { 5,5,5 };//大きさ
	//プレイヤーとの距離感
	Vector3 distance_;

	//リアクション
	Vector3 bombScale_ = { 0.05f, 0.05f, 0.05f };
	const float kScaleMax_ = 0.2f;//スケール最大値
	const float kOnTheVerge = 4.0f;//爆発寸前までのタイマー
	const float kScaleSpeedUp_ = 2.0f;
	/// <summary>
	/// 爆発する
	/// </summary>
	void Exprosion();

	/// <summary>
	/// タイムリミット
	/// </summary>
	void TimeRimmit();
};

