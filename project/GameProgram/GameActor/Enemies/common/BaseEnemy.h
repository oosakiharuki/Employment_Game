#pragma once
#include "EnemyBullet.h"
#include "Player.h"
#include "GameActor.h"

#include "BaseEnemyState.h"
#include "EnemyEye.h"

#include "TimeScale.h"

/// <summary>
/// 敵の基盤クラス
/// </summary>
class BaseEnemy : public GravityActor {
public:

	BaseEnemy();
	virtual ~BaseEnemy();
	/// <summary>
	/// 共有する初期化処理
	/// </summary>
	void Enemy_InitializeCommon(const std::string& objectName);

	/// <summary>
	/// 共有する更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 捜索処理
	/// </summary>
	virtual void SearchCommand() = 0;
	/// <summary>
	/// 攻撃処理
	/// </summary>
	virtual void AttackCommand() = 0;

	/// <summary>
	/// 最後にオブジェクト更新処理
	/// </summary>
	void UpdateBehind();

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
	/// 角度で向きを変更
	/// </summary>
	void DirectionDegree();

	/// <summary>
	/// 見える範囲
	/// </summary>
	void SearchRange();

	/// <summary>
	/// プレイヤーを見つけたフラグ(getter)
	/// </summary>
	/// <returns></returns>
	bool IsFoundTarget() { return enemyEye_->IsFound(); }

	/// <summary>
	/// 見失うフラグ
	/// </summary>
	bool IsLostFound();

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

	void AttackFlag() { attackSwitch_ = true; }

	/// <summary>
	/// ステートパターン変更
	/// </summary>
	/// <param name="enemyState">次のステートパターン</param>
	void ChangeStatePattern(std::unique_ptr<BaseEnemyState> enemyState);

protected:

	/// <summary>
	/// imGuiの更新処理
	/// </summary>
	virtual void UpdateImGui() = 0;

	/// <summary>
	/// 見つかけた時のリアクション処理
	/// </summary>
	void FoundReaction();
	/// <summary>
	/// 倒されたリアクション処理
	/// </summary>
	void DeadReaction();

	/// <summary>
	/// ステート変更(捜索(search)、攻撃(attack))
	/// </summary>
	void StatePatternUpdate();

	//オブジェクト
	std::unique_ptr<Object_glTF> object_;

	//プレイヤークラス
	Player* player_ = nullptr;

	Vector3 eyeReach_{};

	//ゲームに移さないフラグ
	bool isDeleteEnemy_ = false;
	//倒された時の回転リアクション
	const float kDeadRotation_ = 4.0f;


	//パーティクル
	//ダメージを食らったとき
	const std::string& particleDamage_ = "enemy_damage";

	const float kMarkMaxTime_ = 1.0f;
	float markTimer_ = 0.0f;

	//見つかけたリアクションのパラメータ
	bool isFoundReaction_ = false;
	const float kFoundReactionMaxTime_ = kMarkMaxTime_ / 5.0f;//リアクションで伸び縮みする時間
	Vector3 preTranslate_;//リアクション前の座標
	float foundTimer_ = 0.0f;//見つけるモーションの時間

	std::unique_ptr<EnemyEye> enemyEye_ = nullptr;

	bool attackSwitch_ = false;
	
private:

	//ステートパターン
	std::unique_ptr<BaseEnemyState> enemyState_ = std::make_unique<EnemySearchState>();//ここでも初期化できる

	std::unique_ptr<Object3d> objectFound_;
	std::unique_ptr<Object3d> objectNoFound_;

	WorldTransform wtMark_;
	Transform transformMark_{};

	const float kMarkPositionY_ = 2.0f;

	const float kFoundMotionMaxTime_ = kMarkMaxTime_ / 5.0f;

	//体力スプライト
	std::unique_ptr<Sprite> hpSprite_;
	std::unique_ptr<Sprite> underBarSprite_;
	//パラメータ
	const Vector2 kSpriteWindowsPosition_ = { 0.7f,0.85f };
	const Vector2 kHpSpriteSize_ = { 256,64 };
	const float kSpriteRatio_ = 0.1f;

	const float kMaxHpSpriteTimer_ = 1.5f;
	float hpSpriteTimer_ = kMaxHpSpriteTimer_;

	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;
	/// <summary>
	/// 当たり判定をとるタイプかをチェック
	/// </summary>
	/// <param name="collisionType">相手の当たり判定タイプ</param>
	/// <returns>該当するタイプがあるなら true</returns>
	bool TypeCheckUp(const CollisionTypes& collisionType) override;

	/// <summary>
	/// 体力バースプライト更新	 
	/// </summary>
	void HpSpriteUpdate();
};
