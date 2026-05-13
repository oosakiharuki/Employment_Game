#pragma once
#include "BaseUmbrella.h"

class FoldingUmbrella : public BaseUmbrella {
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

	uint32_t rapidCount_ = 0;
	const uint32_t kMaxRapidCount_ = 3;

	const float kRapidTimeMax_ = 0.1f;
	float rapidTimer_ = kRapidTimeMax_;

	const uint32_t kBulletPower_ = 1;//弾丸の攻撃力

	const float kBulletSpeed_ = 0.75f;//弾丸の前方向の速さ
	const float kBulletPowerUpSpeed_ = 1.5f;
};

