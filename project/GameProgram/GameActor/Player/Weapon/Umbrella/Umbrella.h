#pragma once
#include "Object_glTF.h"
#include "BaseUmbrella.h"

/// <summary>
/// 傘(発泡、守が使える)
/// </summary>
class Umbrella : public BaseUmbrella {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Umbrella();
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
	const Vector3 kAABBSize_ = { 1,2,1 };

	const float kDispersionBetween_ = 0.1f;//分散する間
	const float kBulletSpeed_ = 0.5f;//弾丸の前方向の速さ

	const uint32_t kBulletCount_ = 3;//一度に出る弾丸数

	//真ん中を0にする値(3の場合、1,0,-1 | 5の場合、2,1,0,-1,-2)
	float halfCount = float((kBulletCount_ - 1) * 0.5f);//二で割る
	const uint32_t kBulletPower_ = 1;//弾丸の攻撃力

	const Vector3 kBulletKnockbackPower_ = { 0.0f,0.0f,0.1f };//撃った場合のノックバックパワー
};

