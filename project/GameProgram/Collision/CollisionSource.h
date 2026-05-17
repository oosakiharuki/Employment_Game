#pragma once
#include "MyMath.h"

/// <summary>
/// enum - 当たり判定のタイプ
/// </summary>
enum CollisionTypes {
	TypePlayer,          //プレイヤー
	TypePlayerBullet,    //プレイヤーの弾丸
	TypeUmbrella,        //傘
	TypeUmbrellaParry,   //パリィ状態の傘
	TypeEnemy,           //敵
	TypeEnemyEye,        //敵の視野
	TypeEnemyDamageBody, //体にダメージがある敵
	TypeEnemyBullet,     //敵の弾丸
	TypeStageObject,     //ステージオブジェクト
	TypeShadow,          //影
	TypeStage,           //ステージ全体
	TypeMoveGround,      //動く床
	TypeEnergy,          //エナジー
	TypeBoss,            //ボス
	TypeBombExplotion,   //爆弾の爆風
	TypeEvent,           //強制イベント
};

/// <summary>
/// 当たり判定の衝突の反応
/// </summary>
class CollisionSource {
public:
	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns>AABB</returns>
	AABB GetAABB() { return collisionAABB_; }
	/// <summary>
	/// setter_当たり判定AABB
	/// </summary>
	/// <param name="aabb">設定したAABB</param>
	void SetAABB(const AABB& aabb) { collisionAABB_ = aabb; }
	/// <summary>
	/// getter_当たり判定のタイプ
	/// </summary>
	/// <returns>当たり判定のタイプ</returns>
	CollisionTypes GetType() { return collisionType_; }
	/// <summary>
	/// setter_当たり判定のタイプ
	/// </summary>
	/// <param name="type">設定するタイプ(CollisionTypes)</param>
	void SetType(const CollisionTypes& type) { collisionType_ = type; }
	/// <summary>
	/// getter_センター
	/// </summary>
	/// <returns>真ん中の座標</returns>
	const Vector3& GetCenter() { return center_; }
	/// <summary>
	/// setter_センター
	/// </summary>
	/// <param name="center">真ん中座標の設定</param>
	void SetCenter(const Vector3& center) { center_ = center; }
	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	virtual void OnCollision(CollisionSource* collision);
	/// <summary>
	/// 当たり判定をとるタイプかをチェック
	/// </summary>
	/// <param name="collisionType">相手の当たり判定タイプ</param>
	/// <returns>該当するタイプがあるなら true</returns>
	virtual bool TypeCheckUp(const CollisionTypes& collisionType);

protected:
	CollisionTypes collisionType_;//タイプ
	AABB collisionAABB_;          //当たり判定AABB
	Vector3 center_;              //センター(真ん中座標)
};


