#pragma once
#include "IEnemy.h"
#include "MyMath.h"

/// <summary>
/// ボムの敵(IEnemyの派生クラス)
/// </summary>
class Enemy_Bomb : public IEnemy {
public:
	~Enemy_Bomb() override;
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理_通常
	/// </summary>
	void UpdateNormal() override;
	/// <summary>
	/// 更新処理_攻撃中
	/// </summary>
	void UpdateAttack() override;
	/// <summary>
	/// 更新処理_死亡中
	/// </summary>
	void UpdateDead() override;
	/// <summary>
	/// 更新処理_Imgui
	/// </summary>
	void UpdateImgui() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
	/// <summary>
	/// 攻撃
	/// </summary>
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

	void DirectionPlayer();

private:
	//追尾開始
	bool isTuibiStart_ = false;
	//爆発
	bool isExplosion_ = false;

	//爆発するまでのタイマー
	const float kBombTimeMax_ = 5.0f;//max
	float bombTimer_ = 0.0f;
	
	//Hp
	const uint32_t kHp_ = 1;
	//見える範囲初期化
	const Vector3 kEyeReach_ = { 15, 2, 1 };

	//爆発範囲AABB
	AABB bombAABB_;
	const Vector3 kExplosionRange_ = { 5,5,5 };//大きさ
	//プレイヤーとの距離感
	Vector3 distance_;
	const float kSpeed_ = -0.15f;

	//リアクション
	Vector3 bombScale_ = { 0.05f, 0.05f, 0.05f };
	const float kScaleMax_ = 0.2f;//スケール最大値
	const float kOnTheVerge = 4.0f;//爆発寸前までのタイマー
	const float kScaleSpeedUp_ = 2.0f;

	float deadTimer_ = 0.0f;
	const float kDeadTimeMax_ = 1.0f;

	/// <summary>
	/// 爆発する
	/// </summary>
	void Exprosion();

	/// <summary>
	/// タイムリミット
	/// </summary>
	void TimeRimmit();
};

