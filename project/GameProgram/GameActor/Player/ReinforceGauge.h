#pragma once
#include "MyMath.h"
#include "Sprite.h"
#include "Reaction.h"

/// <summary>
/// 強化攻撃ゲージ
/// </summary>
class ReinforceGauge
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	///// <summary>
	///// 描画処理
	///// </summary>
	//void Draw();

	/// <summary>
	/// ポイント加算
	/// </summary>
	void AddPoint();

	/// <summary>
	/// ポイント加算
	/// </summary>
	void UsePoint() {
		gaugePoint_ -= kUseGaugeCost_;//コスト分減算
	}

	/// <summary>
	/// 強化行動の使用権限
	/// </summary>
	/// <returns>強化行動を使用するコスト分ポイントがあるか(3コスト)</returns>
	bool UseGaugePoint() { return gaugePoint_ >= kUseGaugeCost_; }

private:
	std::unique_ptr<Sprite> gaugeSprite_ = nullptr;//ゲージそのもの
	std::vector<std::unique_ptr<Sprite>> gaugePointSprites_;//ポイントを表すスプライトたち
	const uint32_t kGaugePointSpriteNum_ = 4;//最大4つ

	uint32_t gaugePoint_ = 0;//ポイント
	const uint32_t kUseGaugeCost_ = 3;//強化行動コスト、3コスト使う
	const uint32_t kGaugePointMax_ = 12;//最大ポイント保持数([1チャージ 3ポイント] * 4)

	//テクスチャのパラメータ
	const Vector2 GaugePosition_ = { 20.0f,120.0f };//場所
	const float kTextureSize_ = 64.0f;//サイズ

	//リアクションパラメータ
	std::unique_ptr<Reaction> reaction_;
	Vector2 moveSize_;//変更する大きさ
	Vector2 moveSizePower_ = {2.5f,2.5f};//大きくする強さ
	bool isChargeSuccess_ = false;//3コスト分ポイントがたまったフラグ
	float timer_ = 0.0f;//時間
	float timerMax_ = 0.25f;//最大時間
};

