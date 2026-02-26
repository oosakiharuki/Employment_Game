#pragma once
#include "BaseEnemy.h"
#include "MyMath.h"
#include "EnemyCommand.h"

/// <summary>
/// ボムの敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Bomb : public BaseEnemy, public EnemyMoveCommand, public EnemyExplosionCommand, public GravityActor {
public:
	~Enemy_Bomb() override;
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理_ImGui
	/// </summary>
	void UpdateImGui() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

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

	/// <summary>
	/// プレイヤーの位置を測る
	/// </summary>
	void DirectionPlayer();

private:

	void Active() override;
	
	void SearchCommand() override;
	void AttackCommand() override;
	
	void Dead() override;
	void Performance() override;


	void OnCollision(CollisionSource* collision) override;


	/// <summary>
	/// 移動
	/// </summary>
	void Move() override;

	//Hp
	const uint32_t kHp_ = 1;
	//見える範囲初期化
	const Vector3 kEyeReach_ = { 15, 2, 1 };

	//プレイヤーとの距離感
	Vector3 distance_;
	const float kSpeed_ = -0.15f;

	/// <summary>
	/// 爆発する
	/// </summary>
	void Explosion() override;

	/// <summary>
	/// タイムリミット
	/// </summary>
	void TimeLimit() override;

	/// <summary>
	/// 赤の点滅
	/// </summary>
	void RedBlinking() override;
};

