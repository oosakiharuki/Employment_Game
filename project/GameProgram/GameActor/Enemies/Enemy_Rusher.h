#pragma once
#include "BaseEnemy.h"
class Enemy_Rusher : public BaseEnemy {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy_Rusher() override;
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
	/// 予備動作(チャージ)
	/// </summary>
	void Charge();
	/// <summary>
	/// まっすぐ飛ぶ
	/// </summary>
	void RushStreet();
	/// <summary>
	/// 元の位置に戻る
	/// </summary>
	void ReturnPosition();

	/// <summary>
	/// 予備動作の時間がたった場合
	/// </summary>
	/// <returns>たった場合はtrue</returns>
	bool ChargeMax();

	bool isRush_ = false;//突進フラグ
	bool isReturnPosition_ = false;//壁に衝突フラグ

	//予備動作(チャージ)
	float chargeTimer_ = 0.0f;
	const float kChargeTimeMax_ = 1.0f;
	const float kStepBackSpeed_ = 10.0f;//後ろに下げる予備動作、この数値分を割る
	const float kScaleBack_ = 0.25f;

	const Vector3  kRushSpeed_ = { 0.0f,0.0f,0.25f };//突進の速さ
	
	const uint32_t kHp_ = 12;
	const Vector3 kEyeReach_ = { 15.0f,0.5f,0.5f };


	//定位置から衝突した壁までの長さ
	Vector3 lengthMax_;

	//定位置
	Vector3 prevPosition_;
	//壁に衝突した位置
	Vector3 hitWallPosition_;
};

