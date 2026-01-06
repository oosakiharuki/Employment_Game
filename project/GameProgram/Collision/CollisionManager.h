#pragma once
#include "Player.h"
#include "PlayerBullet.h"
#include "BaseEnemy.h"
#include "IStageObject.h"
#include "EventTrigger.h"
#include "Boss.h"
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
	static std::shared_ptr<CollisionManager> GetInstance();

	/// <summary>
	/// プレイヤーと敵の当たり判定
	/// </summary>
	/// <param name="player">プレイヤー</param>
	/// <param name="enemies">敵たち</param>
	void PlayerAndEnemy(Player* player, const std::vector<std::shared_ptr<BaseEnemy>>& enemies);
	
	/// <summary>
	/// プレイヤーとステージオブジェクト
	/// </summary>
	/// <param name="player">プレイヤー</param>
	/// <param name="stageObjects">ステージオブジェクト</param>
	void PlayerAndStageObject(Player* player, const std::list<std::shared_ptr<IStageObject>>& stageObjects);
	
	/// <summary>
	/// プレイヤーとステージ自体
	/// </summary>
	/// <param name="player">プレイヤー</param>
	/// <param name="stagesAABB">ステージ</param>
	void PlayerAndStage(Player* player, const std::vector<AABB>& stagesAABB);
	
	/// <summary>
	/// 敵とステージ自体
	/// </summary>
	/// <param name="enemies">敵たち</param>
	/// <param name="stagesAABB">ステージ</param>
	void EnemyAndStage(const std::vector<std::shared_ptr<BaseEnemy>>& enemies, const std::vector<AABB>& stagesAABB);


	void BossAndPlayer(Player& player, Boss& boss);


	/// <summary>
	/// プレイヤーとイベントトリガー
	/// </summary>
	/// <param name="player">プレイヤー</param>
	/// <param name="eventTriggers">イベントトリガー</param>
	/// <param name="cameraControl_">カメラコントロールクラス</param>
	/// <param name="levelediter">レベルエディタクラス</param>
	void PlayerAndEventTrigger(Player* player, const std::vector<std::shared_ptr<EventTrigger>>& eventTriggers, CameraControl* cameraControl_, Levelediter& levelediter);


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
	/// getter_ワープして次のステージに
	/// </summary>
	/// <returns>ワープフラグ</returns>
	const bool IsWarp() { return isWarp_; }
	/// <summary>
	/// getter_ゴール
	/// </summary>
	/// <returns>ゴールフラグ</returns>
	const bool IsGoal() { return isGoal_; }

	/// <summary>
	/// ゴールとワープのフラグのリセット
	/// </summary>
	void ResetFlag();

private:

	void LookPlayer(Player* player, std::shared_ptr<BaseEnemy> enemy);

	void PlayerBulletAndEnemy(Player* player, std::shared_ptr<BaseEnemy> enemy);

	void EnemyBulletAndPlayer(Player* player, std::shared_ptr<EnemyBullet> bulletE);

	void EnemyAndPariBullet(std::shared_ptr<BaseEnemy> enemy, std::shared_ptr<EnemyBullet> bulletE);

	void EnemyBombCollision(Player* player, std::shared_ptr<BaseEnemy> enemy);



	/// <summary>
	/// 対象の重なった分戻す
	/// </summary>
	/// <param name="collisionBack"></param>
	/// 現在の対象の位置、重なった部分、壁/床のフラグが入ってある構造体
	/// <returns></returns>
	/// 現在の位置から重なる部分を引いた位置に、重なった部分が横なら壁で下なら床のフラグがtrueになる
	void BackPosition(CollisionOverlap& collisionBack);

	/// <summary>
	/// ゲームアクターとステージの当たり判定
	/// </summary>
	/// <param name="gameactor">ゲームアクター(player,enemy)</param>
	/// <param name="stageAABB">ステージ全体当たり判定</param>
	void GameActorAndStageCollision(GameActor& gameactor, const std::vector<AABB>& stageAABB);

	/// <summary>
	/// ステージで作成する当たり判定
	/// </summary>
	/// <param name="collisionOverlap">重なり部分</param>
	/// <param name="stageAABB">ステージ全体当たり判定</param>
	void StageCollisions(CollisionOverlap& collisionOverlap, const std::vector<AABB>& stageAABB);

	/// <summary>
	/// CollisionOverlapのターゲット(player,enemy)の設定
	/// </summary>
	/// <param name="position">ターゲットの座標</param>
	/// <param name="aabb">ターゲット自体の当たり判定</param>
	/// <returns></returns>
	CollisionOverlap SetTarget(const Vector3& position,const AABB& aabb);


	//傘のノックバックの値
	const Vector3 kUmbrellaKnockBackPower_ = { 0.0f,0.0f,0.3f };
	const float kUmbrellaKnockBackTime_ = 0.0f;

	//影で少し上にあげる値
	const float kShadowUp_ = 0.01f;
	//一番真下の値
	const float kMaxUnder = 1000.0f;

	//インスタンス
	static std::shared_ptr<CollisionManager> sInstance;

	//ゴールした時フラグ
	bool isGoal_ = false;
	//ワープで次のステージに進むフラグ
	bool isWarp_ = false;
};

