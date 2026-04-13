#pragma once
#include "CollisionSource.h"
#include "UseEveryOne.h"
#include <memory>
#include <list>

#include "CollisionUtility.h"

/// <summary>
/// ゲーム全体の当たり判定
/// </summary>
class CollisionManager
{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
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

private:

	//インスタンス
	static std::unique_ptr<CollisionManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<CollisionManager>;

	//フレームずつ当たり判定を読み取る
	std::vector<CollisionSource*> collisions_;
	//常に当たり判定を読み取る
	std::vector<std::unique_ptr<CollisionSource>> fixedCollisions_;
	bool isAlready_ = false;//

	//当たり判定(継承なし)
	std::unique_ptr<CollisionSource> collisionTemplate;
};

