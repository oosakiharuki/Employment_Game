#pragma once
#include "BaseEnemy.h"
#include "EnemyCommand.h"

/// <summary>
/// 傭兵の敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Soldier : public BaseEnemy, public EnemyFireCommand,public EnemyMoveCommand, public GravityActor {
public:
	~Enemy_Soldier() override;
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
	/// リスポーン
	/// </summary>
	void RespawnEnemy() override;

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

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void FireBullet() override;

	//速度
	Vector3 velocity_;

	//Hp
	const uint32_t kHp_ = 3;

	//見える範囲初期化
	const Vector3 kEyeReach_ = { 15, 10, 1 };

	//最大弾丸数
	const uint32_t kRapidCountMax_ = 3;
};