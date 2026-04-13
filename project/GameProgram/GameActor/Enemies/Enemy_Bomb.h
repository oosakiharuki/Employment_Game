#pragma once
#include "BaseEnemy.h"
#include "MyMath.h"
#include "EnemyCommand.h"

/// <summary>
/// ボムの敵(BaseEnemyの派生クラス)
/// </summary>
class Enemy_Bomb : public BaseEnemy, public EnemyMoveCommand {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy_Bomb() override;
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理_ImGui
	/// </summary>
	void UpdateImGui() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

private:
	/// <summary>
	/// 生存状態
	/// </summary>
	void Active() override;
	/// <summary>
	/// 捜索態勢
	/// </summary>
	void SearchCommand() override;
	/// <summary>
	/// 攻撃態勢
	/// </summary>
	void AttackCommand() override;
	/// <summary>
	/// 死亡状態
	/// </summary>
	void Dead() override;
	/// <summary>
	/// 演出状態
	/// </summary>
	void Performance() override;
	/// <summary>
	/// 移動
	/// </summary>
	void Move() override;

	/// <summary>
	/// 向きにより最初に移動する方向を決める
	/// </summary>
	void DirectionMove() override;

	/// <summary>
	/// プレイヤーの位置を測る
	/// </summary>
	void DirectionPlayer();

	//Hp
	const uint32_t kHp_ = 1;
	//見える範囲初期化
	const Vector3 kEyeReach_ = { 15, 2, 1 };

	//プレイヤーとの距離感
	Vector3 distance_;
	const float kSpeed_ = -0.15f;


	//追尾開始
	bool isHomingStart_ = false;

	//爆発するまでのタイマー
	const float kBombTimeMax_ = 5.0f;//max
	float bombTimer_ = 0.0f;

	//爆発範囲AABB
	AABB bombAABB_;
	const Vector3 kExplosionRange_ = { 5,5,5 };//大きさ

	//リアクション(拡大縮小)
	Vector3 bombScale_ = { 0.05f, 0.05f, 0.05f };
	const float kScaleMax_ = 0.2f;//スケール最大値
	const float kOnTheVerge = 3.5f;//爆発寸前のタイマー
	const float kScaleSpeedUp_ = 2.0f;
	//リアクション(色)
	Vector4 color_ = { 1,1,1,1 };
	float colorTimer_ = 0.0f;//時間
	float colorTimeMax_ = 0.2f;//色変化の最大時間
	const float kColorChangePower_ = 0.1f;//足し引きするパワー

	float deadTimer_ = 0.0f;
	const float kDeadTimeMax_ = 0.5f;//爆発する間の時間

	/// <summary>
	/// 爆発する
	/// </summary>
	void Explosion();

	/// <summary>
	/// タイムリミット
	/// </summary>
	void TimeLimit();

	/// <summary>
	/// 赤の点滅
	/// </summary>
	void RedBlinking();
};

