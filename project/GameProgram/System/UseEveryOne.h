#pragma once
#include "MyMath.h"

/// <summary>
/// 全てに共通する変数
/// </summary>
namespace UseEveryOne {
	//秒数時間
	const float kDeltaTime_ = 1.0f / 60.0f;
	//2倍( * 2)
	const float kTwice_ = 2.0f;
	//0.5倍(2除算) 半分にするときに有効利用
	const float kDivideByTwo_ = 0.5f;
	//0.33倍(3除算で小数点第二まで)
	const float kDivideByThree_ = 0.33f;
	//元の大きさ
	const Vector3 kDefaultScale_ = { 1,1,1 };

	/// <summary>
	/// 小数点第二までにする[例: 1.234 = 1.23]
	/// </summary>
	/// <param name="value"></param>
	float SecondDecimalPoint(float value);
}
