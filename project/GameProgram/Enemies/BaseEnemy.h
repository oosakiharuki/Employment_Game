#pragma once
#include "EnemyBullet.h"
#include "Player.h"
#include "GameActor.h"

#include "BaseEnemyState.h"

/// <summary>
/// 敵の基盤クラス
/// </summary>
class BaseEnemy : public GameActor{
public:

	BaseEnemy();
	virtual ~BaseEnemy();
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
	void GravityUpdate();

	/// <summary>
	/// 銃弾の処理
	/// </summary>
	/// <returns>銃弾リスト</returns>
	std::list<std::shared_ptr<EnemyBullet>> GetBullets() { return bullets_; }

	/// <summary>
	/// Playerのクラス
	/// </summary>
	/// <param name="player">代入するPlayerクラス</param>
	void SetPlayer(Player* player) { player_ = player; }

	uint32_t GetHp() { return hp_; }

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
	/// 移動ルートのポイント設定:左(現在2つ)
	/// </summary>
	/// <param name="leftPoint">左端</param>
	void SetRouteLeftPoint(const Vector3& leftPoint) { routePointLeft_ = leftPoint; }
	/// <summary>
	/// 移動ルートのポイント設定:右(現在2つ)
	/// </summary>
	/// <param name="leftPoint">右端</param>
	void SetRouteRightPoint(const Vector3& rightPoint) { routePointRight_ = rightPoint; }
	/// <summary>
	/// 移動ルート中心の場合(Eventで使う)
	/// </summary>
	/// <param name="init">真ん中にする場所</param>
	void SetMoveInit(const Vector3& init) { move_ = init; }

	/// <summary>
	/// 角度で向きを変更
	/// </summary>
	void DirectionDegree();

	/// <summary>
	/// setter_ステージ当たり判定
	/// </summary>
	/// <param name="AABBs">現在のステージ当たり判定</param>
	void SetStages(const std::vector<AABB>& AABBs) { stages_ = AABBs; }

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
	/// プレイヤーを見つけたフラグ(getter)
	/// </summary>
	/// <returns></returns>
	bool IsFoundTarget() { return isFoundTarget_; }

	/// <summary>
	/// getter‗爆発範囲AABB
	/// </summary>
	/// <returns>bombAABB</returns>
	virtual AABB GetBombAABB();

	/// <summary>
	/// getter_距離
	/// </summary>
	/// <returns>プレイヤーからボムの距離</returns>
	virtual Vector3 GetDistance();
	
	/// <summary>
	/// 爆発しているか
	/// </summary>
	/// <returns>爆発フラグ</returns>
	virtual bool IsExplosion();

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
	/// imGuiの更新処理
	/// </summary>
	virtual void UpdateImGui() = 0;

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
	const float kCoolTimeMax_ = 2.0f;
	//弾丸の出す間の時間
	float rapidFireTime_ = 0.0f;
	const float kRapidFireTimeMax_ = 0.1f;
	//発射カウント
	uint32_t rapidCount_ = 0;
	//最大弾丸数
	uint32_t rapidCountMax_ = 3;

	//パーティクル
	//ダメージを食らったとき
	ParticleParameters particleDamage_ = {
		"enemy_damage", "resource/Sprite/circle.dds", Primitive::CreateRing(), 20, 0.6f, {1 ,1 ,1}

	};
	//攻撃するとき
	ParticleParameters particleFire_ = {
		"enemy_fire", "resource/Sprite/cone.dds", Primitive::CreateCone(), 1, 0.1f, {1 ,1 ,1}
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
	void FoundReaction();

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
	Transform transformMark_{};

	const float kMarkPositionY_ = 2.0f;

	const float kFoundMotionMaxTime_ = kMarkMaxTime_ / 5.0f;

	//ステートパターン
	std::unique_ptr<BaseEnemyState> enemyState_ = std::make_unique<EnemyMoveState>();

	/// <summary>
	/// 弾丸更新処理
	/// </summary>
	void BulletUpdate();

};
