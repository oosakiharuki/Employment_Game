#pragma once
#include "BaseEnemy.h"
#include "EnemyCommand.h"

/// <summary>
/// ターレットの敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Turret : public BaseEnemy, public EnemyFireCommand, public GravityActor {
public:

	~Enemy_Turret() override;
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

	void RespawnEnemy() override;

private:

	void Active() override;

	void SearchCommand() override;
	void AttackCommand() override;

	void Dead() override;
	void Performance() override;

	void OnCollision(CollisionSource* collision) override;

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void FireBullet()override;

	//レーザーポイントパーティクル
	void LaserPoint();

	Vector3 particleLaserSize_ = { 0.1f,0.1f,0.1f };

	//見える範囲のパーティクルパラメータ
	ParticleParameters particleLaser_ = {
		"tullet_laser","resource/Sprite/3YvXH.dds",Primitive::CreateBeam(), 1, 0.001f, particleLaserSize_
	};

	const Vector3 kParticleFireSize_ = { 1.5f, 1.5f, 1.5f };

	//Hp
	const uint32_t kHp_ = 6;

	//攻撃パーティクルの場所
	Vector3 particlePosition_;

	//見える範囲初期化
	const Vector3 kEyeReach_ = { 20, 0.5f, 0.5f };

	//最大弾丸数
	const uint32_t kRapidCountMax_ = 6;

	//弾丸の出る初期位置設定で使う
	const float kBulletTranslate_ = 1.5f;

	//弾丸速度
	const float kBulletSpeed_ = 0.5f;
};

