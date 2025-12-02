#pragma once
#include "Player.h"
#include "PlayerBullet.h"
#include "IEnemy.h"
#include "IStageObject.h"
#include "EventTrigger.h"
#include "CameraControl.h"

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

class CollisionManager
{
public:

	static std::shared_ptr<CollisionManager> GetInstance();

	/// <summary>
	/// 当たり判定をまとめたもの
	/// </summary>
	/// <param name="player_"></param> プレイヤー
	/// <param name="enemies"></param> エネミー(std::vector)
	/// <param name="stageObjects"></param> ステージオブジェクト(list)
	/// <param name="stagesAABB"></param> ステージの当たり判定
	/// <param name="eventTriggers"></param> イベントトリガー
	/// <param name="cameraControl_"></param> カメラコントロール(イベントトリガーで使う)
	/// <param name="levelediter"></param> レベルエディタ(同じく)
	void AllCollisions(Player* player_, std::vector<std::shared_ptr<IEnemy>> enemies,
		std::list<std::shared_ptr<IStageObject>> stageObjects, std::vector<AABB> stagesAABB,
		std::vector<std::shared_ptr<EventTrigger>> eventTriggers, CameraControl* cameraControl_, Levelediter& levelediter);


	/// <summary>
	/// 対象(プレイヤー、敵など)の真下の床の位置に
	/// 影などで使用する
	/// </summary>
	/// <param name="stageAABB"></param>ステージ地面の全体
	/// <param name="shadowAABB"></param>対象の影
	/// <param name="position"></param>対象の場所
	/// <returns></returns>対象から一番近い地面の上
	Vector3 UnderCollision(std::vector<AABB> stageAABB, AABB shadowAABB, Vector3 position) const;
	
	/// <summary>
	/// getter_ワープして次のステージに
	/// </summary>
	/// <returns></returns>ワープフラグ
	const bool IsWarp() { return isWarp_; }
	/// <summary>
	/// getter_ゴール
	/// </summary>
	/// <returns></returns>ゴールフラグ
	const bool IsGoal() { return isGoal_; }

	/// <summary>
	/// ゴールとワープのフラグのリセット
	/// </summary>
	void ResetFlag();

private:

	/// <summary>
	/// 対象の重なった分戻す
	/// </summary>
	/// <param name="collisionBack"></param>
	/// 現在の対象の位置、重なった部分、壁/床のフラグが入ってある構造体
	/// <returns></returns>
	/// 現在の位置から重なる部分を引いた位置に、重なった部分が横なら壁で下なら床のフラグがtrueになる
	void BackPosition(CollisionOverlap& collisionBack);

	/// <summary>
	/// ステージで作成する当たり判定
	/// </summary>
	/// <param name="collisionOverlap"></param>
	/// <param name="stageAABB"></param>
	void StageCollisions(CollisionOverlap& collisionOverlap, const std::vector<AABB>& stageAABB);

	void GameActorAndStageCollision(GameActor& gameactor, const std::vector<AABB>& stageAABB);

	/// <summary>
	/// CollisionOverlapのターゲット(player,enemy)の設定
	/// </summary>
	/// <param name="position"></param>ターゲットの座標
	/// <param name="aabb"></param>ターゲット自体の当たり判定
	/// <returns></returns>
	CollisionOverlap SetTarget(const Vector3& position,const AABB& aabb);


	//傘のノックバックの値
	const Vector3 kUmbrellaKnockBackPower_ = { 0.0f,0.0f,0.3f };
	const float kUmbrellaKnockBackTime_ = 0.0f;

	//影で少し上にあげる値
	const float kShadowUp_ = 0.01f;

	//インスタンス
	static std::shared_ptr<CollisionManager> sInstance;

	//ゴールした時フラグ
	bool isGoal_ = false;
	//ワープで次のステージに進むフラグ
	bool isWarp_ = false;
};

