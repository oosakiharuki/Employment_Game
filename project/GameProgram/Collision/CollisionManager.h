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

	/// <summary>
	/// 当たり判定を追加(毎フレーム読み取り)
	/// </summary>
	/// <param name="addCollision">コリジョンソース</param>
	void FrameCollision(CollisionSource* addCollision);
	/// <summary>
	/// 当たり判定を追加(毎フレーム読み取り)
	/// 派生クラスCollisionSourceでないとき
	/// </summary>
	/// <param name="collisionAABB">当たり判定AABB</param>
	/// <param name="center">真ん中</param>
	/// <param name="type">当たり判定のタイプ</param>
	void FrameCollision(const AABB& collisionAABB, const Vector3& center, const CollisionTypes& type);

	/// <summary>
	/// 当たり判定を追加(一度読み取り)
	/// </summary>
	/// <param name="addCollision">コリジョンソース</param>
	void FixedCollision(std::unique_ptr<CollisionSource> addCollision);
	/// <summary>
	/// 当たり判定を追加(一度読み取り)
	/// 派生クラスCollisionSourceでないとき
	/// </summary>
	/// <param name="collisionAABB">当たり判定AABB</param>
	/// <param name="center">真ん中</param>
	/// <param name="type">当たり判定のタイプ</param>
	void FixedCollision(const AABB& collisionAABB, const Vector3& center, const CollisionTypes& type);



	/// <summary>
	/// 更新処理
	/// </summary>
	void CollisionUpdate();

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
	void GameActorAndStageCollision(CollisionOverlap& collisionOverlap,GameActor& gameActor, GravityActor& gravityActor,const AABB& otherCollisionAABB);

	/// <summary>
	/// CollisionOverlapのターゲット(player,enemy)の設定
	/// </summary>
	/// <param name="position">ターゲットの現在の座標</param>
	/// <param name="aabb">ターゲット自体の現在の当たり判定</param>
	/// <returns>設定された構造体CollisionOverlap</returns>
	CollisionOverlap SetTarget(const Vector3& position,const AABB& aabb);

	/// <summary>
	/// ワープする判定
	/// </summary>
	/// <returns>する判定ならtrue</returns>
	bool IsWarp() { return isWarp_; }
	/// <summary>
	/// ワープを始める
	/// </summary>
	/// <param name="center">ワープゲートの真ん中座標(ズームして近づけるため)</param>
	void SuccessWarp(const Vector3& center) { 
		isWarp_ = true;
		zoomPoint_ = center;
	}
	/// <summary>
	/// ゴールする判定
	/// </summary>
	/// <returns>する判定ならtrue</returns>
	bool IsGoal() { return isGoal_; }
	/// <summary>
	/// ゴールになる処理
	/// </summary>
	/// <param name="center">ゴールの旗の座標(ズームして近づけるため)</param>
	void SuccessGoal(const Vector3& center) {
		isGoal_ = true;
		zoomPoint_ = center;
	}
	/// <summary>
	/// ゴール、ワープするフラグをリセット
	/// </summary>
	void ResetFrag() {
		isWarp_ = false;
		isGoal_ = false;
	}
	/// <summary>
	/// getter_ズームポイント
	/// </summary>
	/// <returns>現在のズームポイント</returns>
	const Vector3& GetZoomPoint() { return zoomPoint_; }

private:

	//影で少し上にあげる値
	const float kShadowUp_ = 0.01f;

	//インスタンス
	static std::unique_ptr<CollisionManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<CollisionManager>;

	//ゴールした時フラグ
	bool isGoal_ = false;
	//ワープで次のステージに進むフラグ
	bool isWarp_ = false;
	//ズームポイント
	Vector3 zoomPoint_{};
	//フレームずつ当たり判定を読み取る
	std::vector<CollisionSource*> collisions_;
	//常に当たり判定を読み取る
	std::vector<std::unique_ptr<CollisionSource>> fixedCollisions_;
	bool isAlready_ = false;//

	//当たり判定(継承なし)
	std::unique_ptr<CollisionSource> collisionTemplate;
};

