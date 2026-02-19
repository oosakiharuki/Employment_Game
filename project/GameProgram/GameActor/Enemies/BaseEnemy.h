#pragma once
#include "EnemyBullet.h"
#include "Player.h"
#include "GameActor.h"

#include "BaseEnemyState.h"

/// <summary>
/// 敵の基盤クラス
/// </summary>
class BaseEnemy : public GameActor, public EnemyCommand{
public:

	BaseEnemy();
	virtual ~BaseEnemy();
	///// <summary>
	///// 初期化処理
	///// </summary>
	//virtual void Initialize() = 0;
	/// <summary>
	/// 共有する初期化処理
	/// </summary>
	void Enemy_InitializeCommon(const std::string& objectName);

	/// <summary>
	/// 共有する更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 最後にオブジェクト更新処理
	/// </summary>
	void UpdateBehind();

	///// <summary>
	///// 描画処理
	///// </summary>
	//virtual void Draw() = 0;
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
	/// リスポーン
	/// </summary>
	virtual void RespawnEnemy() = 0;

	/// <summary>
	/// 共通リスポーン(体力や初期位置などを戻す)
	/// </summary>
	void RespawnEnemyCommon();

	/// <summary>
	/// 角度で向きを変更
	/// </summary>
	void DirectionDegree();

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

	//オブジェクト
	std::unique_ptr<Object3d> object_;

	//プレイヤークラス
	Player* player_ = nullptr;

	AABB eyeAABB_;//見える範囲
	Vector3 eyeReach_{};
	bool isFoundTarget_ = false;

	//ゲームに移さないフラグ
	bool isDeleteEnemy_ = false;
	//倒された時の回転リアクション
	const float kDeadRotation_ = 4.0f;


	//パーティクル
	//ダメージを食らったとき
	ParticleParameters particleDamage_ = {
		"enemy_damage", "resource/Sprite/circle.dds", Primitive::CreateRing(), 20, 0.6f, {1 ,1 ,1}
	};


	const float kMarkMaxTime_ = 1.0f;
	float markTimer_ = 0.0f;

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

	/// <summary>
	/// ステート変更(捜索(search)、攻撃(attack))
	/// </summary>
	void StatePatternUpdate();

private:

	void OnCollision(CollisionSource* collision) override;

	//ステートパターン
	std::unique_ptr<BaseEnemyState> enemyState_ = std::make_unique<EnemySearchState>();//ここでも初期化できる

	std::unique_ptr<Object3d> objectFound_;
	std::unique_ptr<Object3d> objectNoFound_;

	WorldTransform wtMark_;
	Transform transformMark_{};

	const float kMarkPositionY_ = 2.0f;

	const float kFoundMotionMaxTime_ = kMarkMaxTime_ / 5.0f;

};
