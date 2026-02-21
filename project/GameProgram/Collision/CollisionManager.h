#pragma once
#include "CollisionSource.h"
#include "UseEveryOne.h"
#include <memory>
#include <list>

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
/// ゲーム全体の当たり判定
/// </summary>
class CollisionManager
{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static CollisionManager& GetInstance();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	void AddCollisions(CollisionSource* addCollision);

	void CreateCollision(const AABB& collisionAABB, const Vector3& center, const CollisionTypes& type);

	void CreateStageCollision(const AABB& collisionAABB, const Vector3& center, const CollisionTypes& type);

	void CollisionUpdate();

	void EachCollision(CollisionSource& collisionA, CollisionSource& collisionB);


	/// <summary>
	/// 対象(プレイヤー、敵など)の真下の床の位置に
	/// 影などで使用する
	/// </summary>
	/// <param name="stageAABB">ステージ地面の全体</param>
	/// <param name="shadowAABB">対象の影</param>
	/// <param name="position">対象の場所</param>
	/// <returns>対象から一番近い地面の上</returns>
	Vector3 UnderCollision(const std::vector<AABB>& stageAABB, const AABB& shadowAABB, const Vector3& position) const;

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
	void GameActorAndStageCollision(CollisionOverlap& collisionOverlap,GameActor& gameActor, GravityActor& gravityActor,const AABB& otherCollisionAABB);

	/// <summary>
	/// ステージで作成する当たり判定
	/// </summary>
	/// <param name="collisionOverlap">重なり部分</param>
	/// <param name="stageAABB">ステージ全体当たり判定</param>
	//void StageCollisions(CollisionOverlap& collisionOverlap);

	/// <summary>
	/// CollisionOverlapのターゲット(player,enemy)の設定
	/// </summary>
	/// <param name="position">ターゲットの現在の座標</param>
	/// <param name="aabb">ターゲット自体の現在の当たり判定</param>
	/// <returns>設定された構造体CollisionOverlap</returns>
	CollisionOverlap SetTarget(const Vector3& position,const AABB& aabb);


	bool IsWarp() { return isWarp_; }
	void SuccessWarp() { isWarp_ = true; }

	bool IsGoal() { return isGoal_; }
	void SuccessGoal() { isGoal_ = true; }

	void ResetFrag() {
		isWarp_ = false;
		isGoal_ = false;
	}

private:	
	void DetermineType(CollisionSource& collisionA, const CollisionTypes& typeA, CollisionSource& collisionB, const CollisionTypes& typeB);

	//傘のノックバックの値
	const Vector3 kUmbrellaKnockBackPower_ = { 0.0f,0.0f,0.3f };
	const float kUmbrellaKnockBackTime_ = 0.0f;

	//影で少し上にあげる値
	const float kShadowUp_ = 0.01f;
	//一番真下の値
	const float kMaxUnder = 1000.0f;

	//インスタンス
	static std::unique_ptr<CollisionManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<CollisionManager>;

	//ゴールした時フラグ
	bool isGoal_ = false;
	//ワープで次のステージに進むフラグ
	bool isWarp_ = false;

	std::vector<CollisionSource*> collisions_;

	//ステージ用
	std::vector<std::unique_ptr<CollisionSource>> stageCollisions_;
	bool isAlreadyInStage_ = false;//すでにステージ入っている
};

