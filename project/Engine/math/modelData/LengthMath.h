#pragma once
#include "Vector3.h"
#include "Segment.h"

namespace MyMath {
	/// <summary>
	/// 長さ
	/// </summary>
	/// <param name="start">現在位置</param>
	/// <param name="target">ターゲット</param>
	/// <returns>スタートとターゲットの長さ</returns>
	float Length(float start, float target);
	/// <summary>
	/// 長さ_Vector3版
	/// </summary>
	/// <param name="start">現在位置</param>
	/// <param name="target">ターゲット</param>
	/// <returns>スタートとターゲットの長さ</returns>
	Vector3 Length(const Vector3& start, const Vector3& target);
	
	/// <summary>
	/// 距離の補間
	/// </summary>
	/// <param name="segment">セグメント</param>
	/// <returns>現在位置(origin)と目的地(diff)の距離、マイナスも含む</returns>
	Vector3 GoDestination(const Segment& segment);
	/// <summary>
	/// 距離の補間(分離版)
	/// </summary>
	/// <param name="origin">現在地</param>
	/// <param name="diff">目的地</param>
	/// <returns>現在位置(origin)と目的地(diff)の距離、マイナスも含む</returns>
	Vector3 GoDestination(const Vector3& origin, const Vector3& diff);
}

