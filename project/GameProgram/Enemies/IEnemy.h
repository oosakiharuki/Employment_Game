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
	void Enemy_InitializeCommon(const std::string& objectName);

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
	/// パーティクル用の描画処理
	/// </summary>
	void DrawParticle();

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
	std::list<std::shared_ptr<EnemyBullet>> GetBullets() { return bullets_; }

	/// <summary>
	/// Playerのクラス
	/// </summary>
	/// <param name="player"></param>代入するPlayerクラス
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 完全に削除する(チェックポイントで消せる)
	/// </summary>
	/// <returns></returns>
	bool GetDeleteEnemy() { return isDeleteEnemy_; }

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
	void SetRoutePoint1(Vector3 point1) { routePointLeft_ = point1; }
	void SetRoutePoint2(Vector3 point2) { routePointRight_ = point2; }
	/// <summary>
	/// 移動ルート中心の場合(Eventで使う)
	/// </summary>
	/// <param name="init"></param>真ん中にする場所
	void SetMoveInit(Vector3 init) { move_ = init; }

	/// <summary>
	/// 角度で向きを変更
	/// </summary>
	void DirectionDegree();

	/// <summary>
	/// setter_ステージ当たり判定
	/// </summary>
	/// <param name="aabb"></param>
	void SetStages(std::vector<AABB> aabb) { stages_ = aabb; }

	/// <summary>
	/// 影の座標位置
	/// </summary>
	/// <param name="position"></param>代入する座標位置(プレイヤーに一番近い床)
	void SetShadowPosition(Vector3 position) { shadow_->SetTranslate(position); }
	
	/// <summary>
	/// 見える範囲
	/// </summary>
	void SearchRange();

	/// <summary>
	/// 動く敵
	/// </summary>
	void MoveEnemy();

	/// <summary>
	/// getter_見える範囲
	/// </summary>
	/// <returns></returns>
	AABB GetEyeAABB() { return eyeAABB_; }

	/// <summary>
	/// プレイヤーを見つけたフラグ
	/// </summary>
	/// <param name="result"></param>
	void IsFoundTarget(bool result) { isFoundTarget_ = result; }

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

	/// <summary>
	/// 爆発し終わったら
	/// </summary>
	virtual void ExplosionEnd();

	/// <summary>
	/// 発泡
	/// </summary>
	void Fire();

	/// <summary>
	/// 発泡弾
	/// </summary>
	virtual void FireBullet();

protected:
	//オブジェクト
	std::unique_ptr<Object3d> object_;

	//弾丸リスト
	std::list<std::shared_ptr<EnemyBullet>> bullets_;
	//プレイヤークラス
	Player* player_ = nullptr;
	
	//playerを見つけたとき
	void PlayerTarget();
	AABB eyeAABB_;//見える範囲
	Vector3 eyeReach_{};
	bool isFoundTarget_ = false;

	//敵を倒した時のUpdate関数
	void DeadUpdate();

	//倒された時フラグ
	bool isDeleteEnemy_ = false;//完全に削除フラグ

	//動く範囲
	Vector3 routePointRight_;
	Vector3 routePointLeft_;

	//弾丸発射フラグ
	bool isBullet_ = false;
	//クールタイム
	float coolTime_ = 0.0f;
	const float kCoolTimeMax_ = 1.25f;
	//弾丸の出す間の時間
	float rapidFireTime_ = 0.0f;
	const float kRapidFireTimeMax_ = 0.1f;
	//発射カウント
	uint32_t rapidCount_ = 0;
	//最大弾丸数
	uint32_t rapidCountMax_ = 3;

	//パーティクル
	//ダメージを食らったとき
	ParticleParametars particleDamage_ = {
		"enemy_damage", "resource/Sprite/circle.png", PrimitiveType::ring, 20, 0.6f, { 1 ,1 ,1 }

	};
	//攻撃するとき
	ParticleParametars particleFire_ = {
		"enemy_fire", "resource/Sprite/cone.png", PrimitiveType::cone, 1, 0.1f, { 1 ,1 ,1 }
	};

	//移動するとき
	Vector3 speed_ = { 0,0,0 };
	//移動
	Vector3 move_ = { 0,0,0 };
	const float kMoveX_ = 0.03f;

	const float kMarkMaxTime_ = 1.0f;
	float markTimer_ = 0.0f;
	bool isLostPlayer_ = false;

private:
	std::vector<AABB> stages_;

	std::unique_ptr<Object3d> objectFound_;
	std::unique_ptr<Object3d> objectNoFound_;

	WorldTransform wtMark_;
	const float kMarkPositionY_ = 2.0f;

	uint32_t i = 0;
};

