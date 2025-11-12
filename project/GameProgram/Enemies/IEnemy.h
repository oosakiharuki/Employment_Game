#pragma once
#include "EnemyBullet.h"
#include "Player.h"
#include "GameActor.h"

/// <summary>
/// 敵の基盤クラス
/// </summary>
class IEnemy : public GameActor{
public:

	IEnemy();
	virtual ~IEnemy();
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;
	/// <summary>
	/// 共有する初期化処理
	/// </summary>
	void Enemy_InitializeCommon();

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 共有する更新処理
	/// </summary>
	void UpdateCommon();

	/// <summary>
	/// 最後にオブジェクト更新処理
	/// </summary>
	void UpdateBehind();

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;
	/// <summary>
	/// 共有する更新処理
	/// </summary>
	void DrawCommon();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	virtual void Attack() = 0;

	/// <summary>
	/// ダメージ
	/// </summary>
	void IsDamage();

	/// <summary>
	/// 重力更新処理
	/// </summary>
	void GrabityUpdate();

	/// <summary>
	/// 銃弾の処理
	/// </summary>
	/// <returns></returns>銃弾リスト
	std::list<EnemyBullet*> GetBullets() { return bullets_; }

	/// <summary>
	/// Playerのクラス
	/// </summary>
	/// <param name="player"></param>代入するPlayerクラス
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 完全に削除する(チェックポイントで消せる)
	/// </summary>
	/// <returns></returns>
	bool GetDeleteEnemy() { return deleteEnemy; }

	/// <summary>
	/// リスポーン
	/// </summary>
	virtual void RespawnEnemy() = 0;

	/// <summary>
	/// 共通リスポーン(体力や初期位置などを戻す)
	/// </summary>
	void RespawnEnemyCommon();

	/// <summary>
	/// 移動ルートのポイント設定(現在2つ)
	/// </summary>
	/// <param name="point1"></param>
	void SetRoutePoint1(Vector3 point1) { route_point1 = point1; }
	void SetRoutePoint2(Vector3 point2) { route_point2 = point2; }
	/// <summary>
	/// 移動ルート中心の場合(Eventで使う)
	/// </summary>
	/// <param name="init"></param>真ん中にする場所
	void SetMoveInit(Vector3 init) { move = init; }

	/// <summary>
	/// 角度で向きを変更
	/// </summary>
	void DirectionDegree();

	void SetStages(std::vector<AABB> aabb) { stages = aabb; }

	/// <summary>
	/// 影の座標位置
	/// </summary>
	/// <param name="position"></param>代入する座標位置(プレイヤーに一番近い床)
	void SetShadowPosition(Vector3 position) { shadow_->SetTranslate(position); }

	void SearchRange();

	/// <summary>
	/// 動く敵
	/// </summary>
	void MoveEnemy();

	/// <summary>
	/// getter_見える範囲
	/// </summary>
	/// <returns></returns>
	AABB GetEyeAABB() { return eyeAABB; }

	void IsFoundTarget(bool result) { isFoundTarget = result; }

	/// <summary>
	/// getter‗爆発範囲AABB
	/// </summary>
	/// <returns></returns>bombAABB
	virtual AABB GetBombAABB();

	/// <summary>
	/// getter_距離
	/// </summary>
	/// <returns></returns>プレイヤーからボムの距離
	virtual Vector3 GetDistance();
	
	/// <summary>
	/// 爆発
	/// </summary>
	/// <returns></returns>爆発フラグ
	virtual bool IsExplosion();

	void Fire();

	virtual void FireBullet();

protected:
	//オブジェクト
	std::unique_ptr<Object3d> object;

	//弾丸リスト
	std::list<EnemyBullet*> bullets_;
	//プレイヤークラス
	Player* player_ = nullptr;
	
	//playerを見つけたとき
	void PlayerTarget();
	AABB eyeAABB;//見える範囲
	Vector3 eyeReach{};
	bool isFoundTarget = false;

	//敵を倒した時のUpdate関数
	void DeadUpdate();

	const float direction_right = 90.0f;
	const float direction_left = -90.0f;

	//倒された時フラグ
	bool deleteEnemy = false;//完全に削除フラグ

	//初期位置保管
	Vector3 init_point;
	Vector3 init_rotate;

	//動く範囲
	Vector3 route_point1;
	Vector3 route_point2;

	////リアクション
	//float scaleTimer = 0.0f;
	//Vector3 defaultScale = { 1,1,1 };//元の大きさ
	////ダメージのリアクション
	//bool isDamageMosion = false;
	//Vector3 damageScale = { 0.1f, 0.1f, 0.1f };
	//const float damageMaxTime = 0.14f;


	//弾丸発射フラグ
	bool isBullet = false;
	//クールタイム
	float coolTime = 0.0f;
	const float coolTimeMax = 1.5f;
	//弾丸の出す間の時間
	float rapidFireTime = 0.0f;
	const float rapidFireTimeMax = 0.1f;
	//発射カウント
	uint32_t rapidCount = 0;
	//最大弾丸数
	uint32_t rapidCountMax = 3;

	//パーティクル
	std::unique_ptr<Particle> particle_damage;//ダメージを食らったとき
	std::unique_ptr<Particle> particle_fire;//攻撃するとき

	//移動するとき
	Vector3 speed = { 0,0,0 };
	//移動
	Vector3 move = { 0,0,0 };
	const float moveX = 0.03f;

	const float markMaxTime = 1.0f;
	float markTimer = 0.0f;
	bool lost_player = false;

private:
	std::vector<AABB> stages;

	std::unique_ptr<Object3d> object_found;
	std::unique_ptr<Object3d> object_noFound;

	WorldTransform wtMark;

};

