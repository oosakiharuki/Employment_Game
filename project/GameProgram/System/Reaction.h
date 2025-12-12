#pragma once
#include "MyMath.h"

class Reaction
{
public:
	/// <summary>
	/// リアクション_拡大縮小
	/// </summary>
	/// <param name="value">変えたい値</param>
	/// <param name="mosionOn">モーションフラグ</param>
	/// <param name="power">拡縮する強さ</param>
	/// <param name="timer">時間</param>
	/// <param name="maxTime">最大時間</param>
	void ScaleReaction(Vector3& value,bool& mosionOn, const Vector3& power, float& timer, float maxTime);

	/// <summary>
	/// リアクション_敵がプレイヤーを見つけた
	/// </summary>
	/// <param name="value">変えたい値</param>
	/// <param name="mosion">モーションフラグ</param>
	/// <param name="power">強さ</param>
	/// <param name="timer">時間</param>
	/// <param name="maxTime">最大時間</param>
	/// <param name="prePosition">元の位置</param>
	void FoundReaction(Vector3& value, bool& mosion, const Vector3& power, float& timer, float maxTime, const Vector3& prePosition);

private:

	/// <summary>
	/// 経過時間で足し引き
	/// </summary>
	/// <param name="value">変えたい値</param>
	/// <param name="power">強さ</param>
	/// <param name="timer">時間</param>
	/// <param name="maxTime">最大時間</param>
	void ReturnHalfTime(Vector3& value, const Vector3& power, float& timer, float maxTime);
};

