#pragma once
#include "BaseEnemy.h"
#include "MyMath.h"
#include "EnemyCommand.h"

/// <summary>
/// ボムの敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Bomb : public BaseEnemy, public EnemyMoveCommand, public EnemyExplosionCommand, public GravityActor {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
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
	/// <summary>
	/// 生存状態
	/// </summary>
	void Active() override;
	/// <summary>
	/// 捜索態勢
	/// </summary>
	void SearchCommand() override;
	/// <summary>
	/// 攻撃態勢
	/// </summary>
	void AttackCommand() override;
	/// <summary>
	/// 死亡状態
	/// </summary>
	void Dead() override;
	/// <summary>
	/// 演出状態
	/// </summary>
	void Performance() override;
	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;
	/// <summary>
	/// 移動
	/// </summary>
	void Move() override;

	/// <summary>
	/// 向きにより最初に移動する方向を決める
	/// </summary>
	void DirectionMove() override;

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

