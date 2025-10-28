#pragma once
#include "IEnemy.h"

/// <summary>
/// ターレットの敵(IEnemyの派生クラス)
/// </summary>
class Enemy_Turret : public IEnemy {
public:

	~Enemy_Turret() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Attack() override;

	/// <summary>
	/// 倒されたフラグ
	/// </summary>
	/// <returns></returns>trueなら倒された
	bool IsDead() { return isDead; }

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void Fire();

	void RespownEnemy() override;

private:
	//発射数
	const uint32_t rapidFireMax = 6;
};

