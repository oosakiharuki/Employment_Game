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
	void Attack() override;

	bool IsDead() { return isDead; }

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void Fire();

	void RespawnEnemy() override;

private:
	//弾丸数
	const uint32_t rapidFireMax = 3;
	//速度
	Vector3 velocity;

};