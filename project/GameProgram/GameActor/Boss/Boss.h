#pragma once
#include "Object_glTF.h"
#include "WorldTransform.h"
#include "MyMath.h"
#include "EnemyBullet.h"

#include "BaseBossState.h"
#include "Player.h"

#include "CollisionManager.h"

/// <summary>
/// ボス(クラッコっぽい動き)
/// </summary>
class Boss : public CollisionSource
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// setter_座標
	/// </summary>
	/// <param name="translate">指定する座標</param>
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	void SetTransform(const Transform& transform) {
		transform_ = transform;
		//センター設定
		move_.origin = transform_.translate;
		kCenter_.y = transform_.translate.y;
	}

	void SetPlayer(Player* player) { player_ = player; }

	Player* GetPlayer() { return player_; }

	/// <summary>
	/// setter_当たり判定
	/// </summary>
	/// <param name="aabb">AABB</param>
	void SetColliderSize(const Vector3& size) { colliderSize_ = size; }

	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns>現在の座標</returns>
	const Vector3& GetTranslate() { return transform_.translate; }

	/// <summary>
	/// getter_弾丸
	/// </summary>
	/// <returns>弾丸たち</returns>
	std::list<std::shared_ptr<EnemyBullet>> GetBullets() { return bullets_; }

	/// <summary>
	/// ダメージ
	/// </summary>
	void IsDamage();

	/// <summary>
	/// 移動ポイントの設定
	/// </summary>
	/// <param name="point">移動させるポイント</param>
	/// <param name="speedDivision">移動の分割数</param>
	void SetMovePoint(const Vector3& point, float speedDivision = 180.0f);

	/// <summary>
	/// getter_行動カウント
	/// </summary>
	/// <returns>カウント数</returns>
	uint32_t ActionCount() { return actionCount_; }
	/// <summary>
	/// 行動カウント
	/// </summary>
	void AddActionCount() { actionCount_++; }
	/// <summary>
	/// 行動カウントリセット
	/// </summary>
	void ResetActionCount() { actionCount_ = 0; }

	/// <summary>
	/// 動く完了フラグリセット
	/// </summary>
	void ResetMoveSuccess() { isMoveSuccess_ = false; }

	void SetOrigin(const Vector3& value) { move_.origin = value; }

	/// <summary>
	/// 発泡処理
	/// </summary>
	/// <param name="kFrame">連射時間の分割</param>
	void Fire(float kFrame, float bulletSpeed, uint32_t bulletMax);
	/// <summary>
	/// 弾丸攻撃
	/// </summary>
	void FireBullet();
	
	/// <summary>
	/// 現在地(origin)から目的地(diff)に着くまでの処理
	/// </summary>
	void ArrivedSegmentDiff();
	/// <summary>
	/// 移動完了
	/// </summary>
	/// <returns></returns>
	bool IsMoveSuccess() { return isMoveSuccess_; }

	/// <summary>
	/// 発砲終了フラグリセット
	/// </summary>
	void ResetStopFire() { isStopFire_ = false; }
	/// <summary>
	/// 発砲終了フラグ
	/// </summary>
	/// <returns>false まだ発砲中 / true 発砲終了</returns>
	bool IsStopFire() { return isStopFire_; }

	/// <summary>
	/// ステートパターン変更処理
	/// </summary>
	/// <param name="playerState">次のステートパターン</param>
	void ChangeStatePattern(std::unique_ptr<BaseBossState> state);

	/// <summary>
	/// 行動前モーション処理
	/// </summary>
	void BeforeActionMotion();

	/// <summary>
	/// 行動前モーション終了フラグ
	/// </summary>
	/// <returns>trueで終了</returns>
	bool IsMotionFinish() { return isMotionFinish_; }

	/// <summary>
	/// 行動前モーション終了フラグリセット
	/// </summary>
	void ResetMotionFinish() { isMotionFinish_ = false; }

	/// <summary>
	/// getter_中心座標
	/// </summary>
	/// <returns>中心座標</returns>
	const Vector3& GetCenter() { return kCenter_; }

	bool IsDead() { return isDead_; }

	bool IsDeadMotion() { return isDeadMotion_; }

	void DeadMotion();

	void DeadPosition();

private:

	void OnCollision(CollisionSource* collision) override;

	std::unique_ptr<Object_glTF> object_;
	WorldTransform wt_;
	Transform transform_{};

	Vector3 colliderSize_;

	std::list<std::shared_ptr<EnemyBullet>> bullets_;
	float bulletSpeed_ = 0.2f;//弾の速さ

	Vector3 kCenter_ = { 0,0,0 };

	Segment move_{};
	float moveFrame_ = 180.0f;//分割

	//攻撃カウント
	uint32_t actionCount_ = 0;

	//体力
	const uint32_t maxHp_ = 60;
	uint32_t hp_ = maxHp_;

	bool isMoveSuccess_ = false;

	///攻撃

	//プレイヤークラス(追尾用)
	Player* player_ = nullptr;

	//発砲終了フラグ
	bool isStopFire_ = false;
	//クールタイム
	float coolTime_ = 0.0f;
	const float kCoolTimeMax_ = 1.25f;
	//弾丸の出す間の時間
	float rapidFireTime_ = 0.0f;
	const float kRapidFireTimeMax_ = 0.1f;
	//発射カウント
	uint32_t rapidCount_ = 0;

	//モーション終了処理
	bool isMotionFinish_ = false;
	const float kRotateOneLap_ = 360.0f;
	const float kRotationX_ = 10.0f;

	//リアクション
	std::unique_ptr<Reaction> reaction_;
	bool isDamageReaction_ = false;
	Vector3 damageReactionPower_ = { 0.05f,0.05f ,0.05f };
	float damageReactionTimer_ = 0.0f;
	const float kDamageReactionTimeMax_ = 0.3f;

	//死んだときの処理
	const float kShakePower = 0.25f;
	Vector3 deadPosition_{};
	Vector3 deadScale_{};
	float deadTimer_ = 0.0f;
	const float kDeadTimeMax_ = 6.0f;
	
	bool isDead_ = false;
	bool isDeadMotion_ = false;

	void ImGuiUpdate();

	//
	Vector3 segmentExtreme = { 0.1f,0.1f,0.1f };

	//ステートパターン
	std::unique_ptr<BaseBossState> bossState_;
};
