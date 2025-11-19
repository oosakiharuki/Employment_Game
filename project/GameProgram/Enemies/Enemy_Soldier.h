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

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void FireBullet() override;

	void RespawnEnemy() override;

private:
	//速度
	Vector3 velocity_;

};