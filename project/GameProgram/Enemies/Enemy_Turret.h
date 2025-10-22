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

	void IsDamage() override;
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
	//弾丸発射フラグ
	bool isBulletStart = false;
	//クールタイム
	float coolTime = 0.0f;
	const float coolTimeMax = 1.0f;
	//弾丸の出す間の時間
	float rapidFireTime = 0.0f;
	const float rapidFireTimeMax = 0.1f;
	//発射数
	uint32_t rapidCount = 0;
	const uint32_t rapidFireMax = 6;
	//パーティクル
	std::unique_ptr<Particle> particle_dead;//倒された時
	std::unique_ptr<Particle> particle_damage;//ダメージを食らったとき
	std::unique_ptr<Particle> particle_fire;//攻撃するとき

	//ダメージのリアクション
	bool isDamageMosion = false;
	Vector3 damageScale = { 0.1f, 0.1f, 0.1f };
	const float damageMaxTime = 0.14f;
};

