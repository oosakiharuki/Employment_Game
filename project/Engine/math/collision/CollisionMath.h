#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "AABB.h"
#include "Segment.h"

namespace MyMath {
	/// <summary>
	/// AABBの衝突判定
	/// </summary>
	/// <param name="aabb1">当たり判定1</param>
	/// <param name="aabb2">当たり判定2</param>
	/// <returns>当たっている場合true</returns>
	bool IsCollisionAABB(const AABB& aabb1, const AABB& aabb2);

	/// <summary>
	/// AABBの衝突判定(内側) 
	/// </summary>
	/// <param name="aabb1">範囲から出られなくするターゲットの当たり判定</param>
	/// <param name="aabb2">当たり判定(範囲の大きさ)</param>
	/// <returns>外側の場合true(外側に出ようとしている)</returns>
	bool IsCollisionAABB_outSide(const AABB& aabb1, const AABB& aabb2);

	/// <summary>
	/// 当たり判定の重なり(ステージの当たり判定で使う)
	/// </summary>
	/// <param name="aabb1">当たり判定1</param>
	/// <param name="aabb2">当たり判定2</param>
	/// <returns>重なりの小さいほう</returns>
	Vector3 OverAABB(const AABB& aabb1, const AABB& aabb2);

	/// <summary>
	/// 目線の当たり判定(ステージを貫通し、見えた判定にならないように)
	/// </summary>
	/// <param name="aabb">当たり判定</param>
	/// <param name="segment">自分の位置(origin)とターゲットの位置(diff)</param>
	/// <returns>セグメント間にAABBがいないか(true/false)</returns>
	bool IsCollisionAABB_Segment(const AABB& aabb, const Segment& segment);

	/// <summary>
	/// 線に当たったか
	/// </summary>
	/// <param name="a1">現在の位置</param>
	/// <param name="a2">ターゲットの位置</param>
	/// <param name="b1"></param>
	/// <param name="b2"></param>
	/// <returns></returns>
	bool Intersect(const Vector2& a1, const Vector2& a2, const Vector2& b1, const Vector2& b2);
}

