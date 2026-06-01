#pragma once
#include "MyMath.h"
#include "GameActor.h"

/// <summary>
/// 重力のあるゲームアクター
/// </summary>
class GravityActor : public GameActor
{
public:
	/// <summary>
	/// 重力の更新処理
	/// </summary>
	/// <param name="translateY">y座標</param>
	/// <param name="isSlowTime">スローをかける場合trueにする</param>
	void GravityUpdate(float& translateY, bool isSlowTime = false);

	/// <summary>
	/// 地面判定フラグ変更
	/// </summary>
	/// <param name="result">trueは地面 / falseは空中</param>
	void IsGround(bool result);

	/// <summary>
	/// getter_地面判定
	/// </summary>
	/// <returns>現在の地面判定</returns>
	bool GetIsGround() { return isGround_; }

protected:

	//地面判定
	bool isGround_ = false;
	//重力
	float gravity_ = 0.0f;
	const float kGravityPower_ = 0.01f;//重力の質量
	const float kGravityMax_ = -1.0f;//最大重力

};

