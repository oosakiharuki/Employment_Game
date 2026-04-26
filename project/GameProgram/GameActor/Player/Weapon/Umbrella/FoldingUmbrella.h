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
	void BornBullet() override;
	/// <summary>
	/// 強い弾丸を発射
	/// </summary>
	void BornPowerBullet() override;

	//AABBのサイズ
	const Vector3 kAABBSize_ = { 1,2,1 };

	uint32_t rapidCount_ = 0;
	const uint32_t kMaxRapidCount_ = 3;

	const float kRapidTimeMax_ = 0.1f;
	float rapidTimer_ = kRapidTimeMax_;
};

