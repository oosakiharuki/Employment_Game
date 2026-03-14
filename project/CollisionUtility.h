#pragma once
#include "MyMath.h"
#include "CollisionManager.h"

class GameActor;
class GravityActor;

/// <summary>
/// 当たり判定:重なり部分を
/// </summary>
struct CollisionOverlap {
	bool isWall = false;
	bool isGround = false;
	Vector3 position = { 0,0,0 };
	Vector3 overlap = { 0,0,0 };
	AABB targetAABB = { { 0,0,0 }, { 0,0,0 } };
	AABB stageAABB = { { 0,0,0 }, { 0,0,0 } };
};

/// <summary>
/// 当たり判定絡みの関数
/// </summary>
class CollisionUtility
{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static CollisionUtility& GetInstance();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 対象(プレイヤー、敵など)の真下の床の位置に
	/// 影などで使用する
	/// </summary>
	/// <param name="minUnder">影のポイント</param>
	/// <param name="actorPosition">ゲームアクターの座標位置</param>
	/// <param name="stageAABB">ステージの当たり判定AABB</param>
	void UnderCollision(float& minUnder, const Vector3& actorPosition, const AABB& stageAABB) const;

	/// <summary>
	/// 対象の重なった分戻す
	/// </summary>
	/// <param name="collisionBack">現在の対象の位置、重なった部分、壁/床のフラグが入ってある構造体</param>
	/// <returns>現在の位置から重なる部分を引いた位置に、重なった部分が横なら壁で下なら床のフラグがtrueになる</returns>
	void BackPosition(CollisionOverlap& collisionBack);

	/// <summary>
	/// ゲームアクターとステージの当たり判定
	/// </summary>
	/// <param name="gameActor">ゲームアクター(player,enemy)</param>
	/// <param name="gravityActor">重力があるゲームアクター</param>
	/// <param name="stageAABB">ステージ全体当たり判定</param>
	void GameActorAndStageCollision(CollisionOverlap& collisionOverlap, GameActor& gameActor, GravityActor& gravityActor, const AABB& otherCollisionAABB);

	/// <summary>
	/// CollisionOverlapのターゲット(player,enemy)の設定
	/// </summary>
	/// <param name="position">ターゲットの現在の座標</param>
	/// <param name="aabb">ターゲット自体の現在の当たり判定</param>
	/// <returns>設定された構造体CollisionOverlap</returns>
	CollisionOverlap SetTarget(const Vector3& position, const AABB& aabb);

	/// <summary>
	/// ワープする判定
	/// </summary>
	/// <returns>する判定ならtrue</returns>
	bool IsWarp() { return isWarp_; }
	/// <summary>
	/// ワープを始める
	/// </summary>
	/// <param name="center">ワープゲートの真ん中座標(ズームして近づけるため)</param>
	void SuccessWarp(const Vector3& center);
	/// <summary>
	/// ゴールする判定
	/// </summary>
	/// <returns>する判定ならtrue</returns>
	bool IsGoal() { return isGoal_; }
	/// <summary>
	/// ゴールになる処理
	/// </summary>
	/// <param name="center">ゴールの旗の座標(ズームして近づけるため)</param>
	void SuccessGoal(const Vector3& center);
	/// <summary>
	/// ゴール、ワープするフラグをリセット
	/// </summary>
	void ResetFrag();
	/// <summary>
	/// getter_ズームポイント
	/// </summary>
	/// <returns>現在のズームポイント</returns>
	const Vector3& GetZoomPoint() { return zoomPoint_; }

private:

	//インスタンス
	static std::unique_ptr<CollisionUtility> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<CollisionUtility>;

	//影で少し上にあげる値
	const float kShadowUp_ = 0.01f;


	//ゴールした時フラグ
	bool isGoal_ = false;
	//ワープで次のステージに進むフラグ
	bool isWarp_ = false;
	//ズームポイント
	Vector3 zoomPoint_{};

	void SetZoomPoint(const Vector3& point);
};

