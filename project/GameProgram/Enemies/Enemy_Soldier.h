#pragma once
#include "BaseEnemy.h"

/// <summary>
/// 傭兵の敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Soldier : public BaseEnemy{
public:
	~Enemy_Soldier() override;
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
	/// 更新処理_ImGui
	/// </summary>
	void UpdateImGui() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void FireBullet() override;

	/// <summary>
	/// リスポーン
	/// </summary>
	void RespawnEnemy() override;

private:
	//速度
	Vector3 velocity_;

	//Hp
	const uint32_t kHp_ = 3;

	//見える範囲初期化
	const Vector3 kEyeReach_ = { 15, 10, 1 };

	//最大弾丸数
	const uint32_t kRapidCountMax_ = 3;
};