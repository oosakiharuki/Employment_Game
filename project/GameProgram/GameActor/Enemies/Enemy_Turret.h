#pragma once
#include "BaseEnemy.h"
#include "EnemyCommand.h"

/// <summary>
/// ターレットの敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Turret : public BaseEnemy , public EnemyCanFireBullet {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
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

private:
	/// <summary>
	/// 生存状態
	/// </summary>
	void Active() override;
	/// <summary>
	/// 捜索態勢
	/// </summary>
	void SearchCommand() override;
	/// <summary>
	/// 攻撃態勢
	/// </summary>
	void AttackCommand() override;
	/// <summary>
	/// 死亡状態
	/// </summary>
	void Dead() override;
	/// <summary>
	/// 演出状態
	/// </summary>
	void Performance() override;

	/// <summary>
	/// 弾丸発射処理
	/// </summary>
	void FireBullet()override;

	//レーザーポイントパーティクル
	void LaserPoint();

	Vector3 particleLaserSize_ = { 0.1f,0.1f,0.1f };

	//見える範囲のパーティクルパラメータ
	const std::string& particleLaser_ = "tullet_laser";

	static constexpr Vector3 kParticleFireSize_ = { 1.5f, 1.5f, 1.5f };

	//Hp
	static constexpr uint32_t kHp_ = 6;

	//攻撃パーティクルの場所
	Vector3 particlePosition_{};

	//見える範囲初期化
	static constexpr Vector3 kEyeReach_ = { 20, 0.5f, 0.5f };

	//最大弾丸数
	static constexpr uint32_t kRapidCountMax_ = 6;

	//弾丸の出る初期位置設定で使う
	static constexpr float kBulletTranslate_ = 1.5f;

	//弾丸速度
	static constexpr float kBulletSpeed_ = 0.5f;

	//発砲攻撃
	std::unique_ptr<EnemyFireCommand> fireCommand_ = nullptr;
};

