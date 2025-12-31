#pragma once
#include "BaseEnemy.h"

/// <summary>
/// ターレットの敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Turret : public BaseEnemy {
public:

	~Enemy_Turret() override;
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
	/// 更新処理_Imgui
	/// </summary>
	void UpdateImgui() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void FireBullet()override;

	void RespawnEnemy() override;

private:

	//レーザーポイントパーティクル
	void LeserPoint();

	Vector3 particleLaserSize_ = { 0.1f,0.1f,0.1f };

	//見える範囲のパーティクルパラメータ
	ParticleParametars particleLaser_ = {
		"tullet_laser","resource/Sprite/3YvXH.png",Primitive::CreateBeam(), 1, 0.001f, particleLaserSize_
	};

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
	const float kBulletTranslate_ = 1.5f;

	//弾丸速度
	const float kBulletSpeed_ = 0.5f;
};

