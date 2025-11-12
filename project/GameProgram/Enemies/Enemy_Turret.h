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
	/// 弾丸発射処理
	/// </summary>
	void FireBullet()override;

	void RespawnEnemy() override;

private:
	//見える範囲のエフェクト
	std::unique_ptr<Particle> particle_laser;

	//攻撃パーティクルの場所
	Vector3 particle_position;
};

