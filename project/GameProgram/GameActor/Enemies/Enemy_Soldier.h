#pragma once
#include "BaseEnemy.h"
#include "EnemyCommand.h"

/// <summary>
/// 傭兵の敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Soldier : public BaseEnemy, public EnemyFireCommand,public EnemyMoveCommand, public GravityActor {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
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