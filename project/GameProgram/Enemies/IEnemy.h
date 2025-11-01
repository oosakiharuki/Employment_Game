#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "EnemyBullet.h"
#include "Player.h"

/// <summary>
/// 敵の基盤クラス
/// </summary>
class IEnemy {
public:

	IEnemy();
	virtual ~IEnemy();
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 共有する更新処理
	/// </summary>
	void UpdateCommon();


	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;
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
	/// getter_座標位置
	/// </summary>
	/// <returns></returns>現在の座標位置
	Vector3 GetTranslate() { return wt.translation_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>代入する座標位置
	void SetTranslate(Vector3 translate) { wt.translation_ = translate;}

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns></returns>現在の回転
	Vector3 GetRotate() { return wt.rotation_; }
	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotate"></param>代入する各回転角度
	void SetRotate(Vector3 rotate) { wt.rotation_ = rotate;}

	/// <summary>
	/// 復活用の初期位置のデータ
	/// </summary>
	/// <param name="translate"></param>座標位置
	/// <param name="rotate"></param>各回転角度
	void SetInit_Position(Vector3 translate, Vector3 rotate) {
		init_point = translate;
		init_rotate = rotate;
	}

	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns></returns>座標位置が真ん中になったAABB判定
	AABB GetAABB();
	/// <summary>
	/// setter_当たり判定
	/// </summary>
	/// <param name="aabb"></param>AABB
	void SetAABB(AABB aabb) { enemyAABB = aabb; }
	/// <summary>
	/// 倒された時(完全に削除ではない)
	/// </summary>
	/// <returns></returns>
	bool IsDead() { return isDead; }

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
	virtual void RespownEnemy() = 0;

	/// <summary>
	/// 共通リスポーン(体力や初期位置などを戻す)
	/// </summary>
	void RespownEnemyCommon();

	/// <summary>
	/// 移動ルートのポイント設定(現在2つ)
	/// </summary>
	/// <param name="point1"></param>
	void SetRoutePoint1(Vector3 point1) { route_point1 = point1; }
	void SetRoutePoint2(Vector3 point2) { route_point2 = point2; }

	/// <summary>
	/// 角度で向きを変更
	/// </summary>
	void DirectionDegree();
	
	/// <summary>
	/// 重力フラグ
	/// </summary>
	void IsGround(bool result) { isGround = result; }

	bool GetIsGround() { return isGround; }

	void SetStages(std::vector<AABB> aabb) { stages = aabb; }

	/// <summary>
	/// 当たったリアクション(拡大縮小)
	/// </summary>
	/// <param name="mosionOn"></param>モーションフラグ
	/// <param name="scale"></param>どのくらい大きくするか
	/// <param name="maxTime"></param>リアクションタイマーの最大値
	void ScaleUpdate(bool* mosionOn,Vector3 scale, const float maxTime);
	
	/// <summary>
	/// 影の設定
	/// </summary>

	/// <summary>
	/// 影の座標位置
	/// </summary>
	/// <param name="position"></param>代入する座標位置(プレイヤーに一番近い床)
	void SetShadowPosition(Vector3 position) { shadow_->SetTranslate(position); }
	/// <summary>
	/// 影の当たり判定
	/// </summary>
	/// <returns></returns>AABB
	AABB GetShadowAABB() { return shadow_->GetAABB(); }
	/// <summary>
	/// 影の更新処理
	/// </summary>
	void ShadowUpdate();

	void isPerformanceFlag(bool result) { isPerformance = result; }

protected:
	//オブジェクト
	std::unique_ptr<Object3d> object;
	//ワールド座標系
	WorldTransform wt;
	//当たり判定AABB
	AABB enemyAABB;

	//弾丸リスト
	std::list<EnemyBullet*> bullets_;
	//プレイヤークラス
	Player* player_ = nullptr;
	
	//playerを見つけたとき
	void PlayerTarget();
	AABB eyeAABB;//見える範囲
	bool isFoundTarget = false;

	//敵を倒した時のUpdate関数
	void DeadUpdate();


	//左右向き
	enum Direction {
		right,
		left
	};
	Direction direction;

	//体力
	uint32_t maxHp;//最大値
	uint32_t hp;

	//倒された時フラグ
	bool isDead = false;//倒れたフラグ
	bool deleteEnemy = false;//完全に削除フラグ

	//初期位置保管
	Vector3 init_point;
	Vector3 init_rotate;

	//動く範囲
	Vector3 route_point1;
	Vector3 route_point2;

	//地面判定
	bool isGround = false;

	//重力
	float grabity = 0.0f;
	
	//秒数時間
	const float deltaTime = 1.0f / 60.0f;
	
	///影
	std::unique_ptr<Shadow> shadow_;

	//リアクション
	float scaleTimer = 0.0f;
	Vector3 defaultScale = { 1,1,1 };//元の大きさ
	//ダメージのリアクション
	bool isDamageMosion = false;
	Vector3 damageScale = { 0.1f, 0.1f, 0.1f };
	const float damageMaxTime = 0.14f;


	//弾丸発射フラグ
	bool isBulletStart = false;
	//クールタイム
	float coolTime = 0.0f;
	const float coolTimeMax = 1.0f;
	//弾丸の出す間の時間
	float rapidFireTime = 0.0f;
	const float rapidFireTimeMax = 0.1f;
	//発射数
	uint32_t rapidCount = 0;


	//パーティクル
	std::unique_ptr<Particle> particle_damage;//ダメージを食らったとき
	std::unique_ptr<Particle> particle_fire;//攻撃するとき

	//演出中フラグ
	bool isPerformance = false;

private:
	std::vector<AABB> stages;
};

