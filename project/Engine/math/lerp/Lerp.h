#pragma once
#include "Vector3.h"
#include "Quaternion.h"

namespace MyMath {

	/// <summary>
	/// 補間
	/// </summary>
	/// <param name="p0">ポイント1(始点)</param>
	/// <param name="p1">ポイント2(終点)</param>
	/// <param name="t">補間単位</param>
	/// <returns>保管した座標</returns>
	Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t);
	
	/// <summary>
	/// 補間_SLerp
	/// </summary>
	/// <param name="q0">制御点0</param>
	/// <param name="q1">制御点1</param>
	/// <param name="t">割合</param>
	/// <returns>現在の位置</returns>
	Quaternion SLerp(const Quaternion& q0, const Quaternion& q1, float t);

}