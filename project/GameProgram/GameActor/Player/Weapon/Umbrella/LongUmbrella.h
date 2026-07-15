#pragma once
#include "BaseUmbrella.h"

/// <summary>
/// 長傘・チャージガン
/// </summary>
class LongUmbrella : public BaseUmbrella {
public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

private:

	/// <summary>
	/// 弾丸を発射
	/// </summary>
	void Fire() override;
	/// <summary>
	/// 強い弾丸を発射
	/// </summary>
	void PowerFire() override;

	//AABBのサイズ
	const Vector3 kAABBSize_ = { 2,2,2 };

	float chargeTimer_ = 0.0f;
	const float kChargeMaxTime_ = 1.0f;

	const float kBulletSpeed_ = 1.0f;//弾丸の前方向の速さ
	const uint32_t kBulletPower_ = 6;//弾丸の攻撃力

	//SE
	EngineLayer::SoundData chargeSound_;

	//パーティクル名
	const std::string& kChargeParticle_ = "longUmbrella_charge";
};
