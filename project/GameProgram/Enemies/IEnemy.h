#pragma once
#include "EnemyBullet.h"
#include "Player.h"
#include "GameActor.h"

#include "BaseEnemyState.h"

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
	/// 共有する更新処理
	/// </summary>
	void Update();

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
	/// <param name="leftPoint"></param>
	void SetRouteleftPoint(const Vector3& leftPoint) { routePointLeft_ = leftPoint; }
	void SetRouterightPoint(const Vector3& rightPoint) { routePointRight_ = rightPoint; }
	/// <summary>
	/// 移動ルート中心の場合(Eventで使う)
	/// </summary>
	/// <param name="init"></param>真ん中にする場所
	void SetMoveInit(const Vector3& init) { move_ = init; }

	/// <summary>
	/// 角度で向きを変更
	/// </summary>
	void DirectionDegree();

	/// <summary>
	/// setter_ステージ当たり判定
	/// </summary>
	/// <param name="aabb"></param>
	void SetStages(const std::vector<AABB>& aabb) { stages_ = aabb; }

	/// <summary>
	/// 影の座標位置
	/// </summary>
	/// <param name="position"></param>代入する座標位置(プレイヤーに一番近い床)
	void SetShadowPosition(const Vector3& position) { shadow_->SetTranslate(position); }
	
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
	/// setter_プレイヤーを見つけたフラグ
	/// </summary>
	/// <param name="result">true:見つけた / false:見つかってない</param>
	void SetFoundTarget(bool result) { isFoundTarget_ = result; }	
	
	/// <summary>
	/// プレイヤーを見つけたフラグ
	/// </summary>
	/// <returns></returns>
	bool IsFoundTarget() { return isFoundTarget_; }

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
	
	void BulletFlag() { isBullet_ = true; }

	/// <summary>
	/// 見失うフラグ
	/// </summary>
	bool IsLostFound();

	/// <summary>
	/// 生きている時の更新処理
	/// </summary>
	virtual void UpdateNormal() = 0;

	/// <summary>
	/// 攻撃中の更新処理
	/// </summary>
	virtual void UpdateAttack() = 0;

	/// <summary>
	/// 死んだ時の更新処理
	/// </summary>
	virtual void UpdateDead() = 0;
	
	/// <summary>
	/// playerを見つけたとき 
	/// </summary>
	void PlayerTarget();

	/// <summary>
	/// !,?のマークの更新処理
	/// </summary>
	void MarkUpdate();
	/// <summary>
	/// !,?のマークの描画処理
	/// </summary>
	void MarkDraw();

	/// <summary>
	/// ステートパターン変更
	/// </summary>
	/// <param name="enemyState">次のステートパターン</param>
	void ChangeStatePattern(std::unique_ptr<BaseEnemyState> enemyState);

	void EnemyFire();

protected:

	/// <summary>
	/// imguiの更新処理
	/// </summary>
	virtual void UpdateImgui() = 0;

	//オブジェクト
	std::unique_ptr<Object3d> object_;

	//弾丸リスト
	std::list<std::shared_ptr<EnemyBullet>> bullets_;
	//プレイヤークラス
	Player* player_ = nullptr;

	AABB eyeAABB_;//見える範囲
	Vector3 eyeReach_{};
	bool isFoundTarget_ = false;

	//ゲームに移さないフラグ
	bool isDeleteEnemy_ = false;
	//倒された時の回転リアクション
	const float kDeadRotation_ = 4.0f;


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
		"enemy_damage", "resource/Sprite/circle.png", Primitive::CreateRing(), 20, 0.6f, {1 ,1 ,1}

	};
	//攻撃するとき
	ParticleParametars particleFire_ = {
		"enemy_fire", "resource/Sprite/cone.png", Primitive::CreateCone(), 1, 0.1f, {1 ,1 ,1}
	};

	//移動するとき
	Vector3 speed_ = { 0,0,0 };
	//移動
	Vector3 move_ = { 0,0,0 };
	const float kMoveX_ = 0.03f;

	const float kMarkMaxTime_ = 1.0f;
	float markTimer_ = 0.0f;
	bool isLostPlayer_ = false;

	/// <summary>
	/// 見つかけた時のリアクション処理
	/// </summary>
	void FoundRiaction();

	//見つかけたリアクションのパラメータ
	bool isFoundReaction_ = false;
	const float kFoundReactionMaxTime_ = kMarkMaxTime_ / 5.0f;//リアクションで伸び縮みする時間
	Vector3 preTranslate_;//リアクション前の座標
	float foundTimer_ = 0.0f;//見つけるモーションの時間

	void DeadReaction();

private:
	std::vector<AABB> stages_;

	std::unique_ptr<Object3d> objectFound_;
	std::unique_ptr<Object3d> objectNoFound_;

	WorldTransform wtMark_;
	const float kMarkPositionY_ = 2.0f;

	const float kFoundMosionMaxTime_ = kMarkMaxTime_ / 5.0f;

	//ステートパターン
	std::unique_ptr<BaseEnemyState> currentEnemyState_ = std::make_unique<EnemyMoveState>();

};
