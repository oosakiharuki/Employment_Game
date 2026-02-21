#pragma once
#include "MyMath.h"

class Reaction
{
public:
	/// <summary>
	/// リアクション_拡大縮小
	/// </summary>
	/// <param name="value">変えたい値</param>
	/// <param name="motionOn">モーションフラグ</param>
	/// <param name="power">拡縮する強さ</param>
	/// <param name="timer">時間</param>
	/// <param name="maxTime">最大時間</param>
	void ScaleReaction(Vector3& value,bool& motionOn, const Vector3& power, float& timer, float maxTime);

	/// <summary>
	/// リアクション_敵がプレイヤーを見つけた
	/// </summary>
	/// <param name="value">変えたい値</param>
	/// <param name="motion">モーションフラグ</param>
	/// <param name="power">強さ</param>
	/// <param name="timer">時間</param>
	/// <param name="maxTime">最大時間</param>
	/// <param name="prePosition">元の位置</param>
	void FoundReaction(Vector3& value, bool& motion, const Vector3& power, float& timer, float maxTime, const Vector3& prePosition);

private:

	/// <summary>
	/// 経過時間で足し引き
	/// </summary>
	/// <param name="value">変えたい値</param>
	/// <param name="power">強さ</param>
	/// <param name="timer">時間</param>
	/// <param name="maxTime">最大時間</param>
	void ReturnHalfTime(Vector3& value, const Vector3& power, float& timer, float maxTime);

	/// <summary>
	/// 終了処理
	/// </summary>
	/// <param name="value">変えたい値</param>
	/// <param name="prePosition">戻す値</param>
	/// <param name="timer">タイマーを0に</param>
	/// <param name="flag">オフにするフラグ</param>
	void Finish(Vector3& value, const Vector3& prePosition, float& timer,bool& flag);
};

