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

	void IsDamage() override;
	bool IsDead() { return isDead; }

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void Fire();

	void RespownEnemy() override;

private:
	//弾丸発射開始フラグ
	bool isBulletStart = false;
	//クールタイム
	float coolTime = 0.0f;
	const float coolTimeMax = 1.0f;
	//弾丸の出す間の時間
	float rapidFireTime = 0.0f;
	const float rapidFireTimeMax = 0.1f;
	//弾丸数
	uint32_t rapidCount = 0;
	const uint32_t rapidFireMax = 3;
	//速度
	Vector3 velocity;

	//移動
	Vector3 speed = { 0.03f,0.0f,0.0f };
	Vector3 move = { 0,0,0 };

	//パーティクル
	std::unique_ptr<Particle> particle_fire;
	std::unique_ptr<Particle> particle_damage;

	//ダメージリアクション
	bool isDamageMosion = false;
	Vector3 damageScale = { 0.1f, 0.1f, 0.1f };
	const float damageMaxTime = 0.14f;
};