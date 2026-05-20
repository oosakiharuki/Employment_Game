#pragma once
#include "Vector2.h"
#include "Vector3.h"

/// <summary>
/// 数学_エンジン
/// </summary>
namespace MyMath {

#pragma region EaseIn
	/// <summary>
	/// イーズイン_float
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	float EaseIn(float startPoint, float endPoint, float t);
	/// <summary>
	/// イーズイン_Vector2
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector2 EaseIn(const Vector2& startPoint, const Vector2& endPoint, float t);
	/// <summary>
	/// イーズイン_Vector3
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector3 EaseIn(const Vector3& startPoint, const Vector3& endPoint, float t);
#pragma endregion

#pragma region EaseOut
	/// <summary>
	/// イーズアウト_float
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	float EaseOut(float startPoint, float endPoint, float t);
	/// <summary>
	/// イーズアウト_Vector2
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector2 EaseOut(const Vector2& startPoint, const Vector2& endPoint, float t);
	/// <summary>
	/// イーズアウト_Vector3
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector3 EaseOut(const Vector3& startPoint, const Vector3& endPoint, float t);
#pragma endregion

#pragma region EaseInOut
	/// <summary>
	/// イーズインアウト_float
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	float EaseInOut(float startPoint, float endPoint, float t);
	/// <summary>
	/// イーズインアウト_Vector2
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector2 EaseInOut(const Vector2& startPoint, const Vector2& endPoint, float t);
	/// <summary>
	/// イーズインアウト_Vector3
	/// </summary>
	/// <param name="startPoint">スタート位置</param>
	/// <param name="endPoint">目的(移動させたい)位置</param>
	/// <param name="t">補間(0~1)</param>
	/// <returns></returns>
	Vector3 EaseInOut(const Vector3& startPoint, const Vector3& endPoint, float t);
#pragma endregion

}