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
	void DrawParticle() override;
	void Attack() override;

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void FireBullet()override;

	void RespawnEnemy() override;

private:
	//見える範囲のエフェクト
	std::unique_ptr<Particle> particleLaser_;
	const uint32_t kParticleLaserCount_ = 1;
	const float kParticleLaserFrequency_ = 0.001f;
	Vector3 particleLaserSize_ = { 0.1f,0.1f,0.1f };

	const Vector3 kParticleFireSize_ = { 1.5f, 1.5f, 1.5f };

	//Hp
	const uint32_t kHp_ = 6;

	//攻撃パーティクルの場所
	Vector3 particlePosition_;

	//見える範囲初期化
	const Vector3 kEyeReach_ = { 20, 0, 0 };

	//最大弾丸数
	const uint32_t kRapidCountMax_ = 6;

	//弾丸の出る初期位置設定で使う
	const float kBulletTranslate_ = 2.0f;

	//弾丸速度
	const float kBulletSpeed_ = 0.5f;
};

