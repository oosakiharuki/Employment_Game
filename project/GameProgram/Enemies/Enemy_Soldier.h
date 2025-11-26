#pragma once
#include "IEnemy.h"

/// <summary>
/// 傭兵の敵(IEnemyの派生クラス)
/// </summary>
class Enemy_Soldier : public IEnemy{
public:
	~Enemy_Soldier() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawParticle() override;
	void Attack() override;

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void FireBullet() override;

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