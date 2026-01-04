#pragma once
#include "Object_glTF.h"
#include "WorldTransform.h"
#include "MyMath.h"
#include "EnemyBullet.h"

#include "BaseBossState.h"
#include "Player.h"
/// <summary>
/// ボス(クラッコっぽい動き)
/// </summary>
class Boss
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
	void SetTranslate(const Vector3& translate) { wt_.translation_ = translate; }

	void SetTransform(const Transform& transform) { 
		wt_.translation_ = transform.translate;
		wt_.rotation_ = transform.rotate;
		wt_.scale_ = transform.scale;

		//センター設定
		move_.origin = wt_.translation_;
		kCenter_ = wt_.translation_;
	}

	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// setter_当たり判定
	/// </summary>
	/// <param name="aabb">AABB</param>
	void SetAABB(const AABB& aabb) { aabb_ = aabb; }

	const AABB& GetAABB();
	
	const Vector3& GetTranslate() { return wt_.translation_; }

	std::list<std::shared_ptr<EnemyBullet>> GetBullets() { return bullets_; }

	/// <summary>
	//右に移動
	/// </summary>
	void MoveRight();
	/// <summary>
	/// 左に移動
	/// </summary>
	void MoveLeft();

	void SetMovePoint(const Vector3& point,float speedBunkatu);


	uint32_t AttackCount() { return attackCount_; }

	void AddAttackCount() { attackCount_++; }

	void ResetAttackCount() { attackCount_ = 0; }

	/// <summary>
	/// 動く完了フラグリセット
	/// </summary>
	void ResetMoveSucces() { isMoveSucces_ = false; }


	/// <summary>
	/// 
	/// </summary>
	void Fire();
	/// <summary>
	/// 弾丸攻撃
	/// </summary>
	void FireBullet();
	/// <summary>
	/// 
	/// </summary>
	/// <param name="isArrived"></param>
	void ArrivedSegmentDiff();

	/// <summary>
	/// 発砲終了フラグリセット
	/// </summary>
	void ResetStopFire() { isStopFire_ = false; }
	/// <summary>
	/// 発砲終了フラグ
	/// </summary>
	/// <returns>false まだ発砲中 / ture 発砲終了</returns>
	bool IsStopFire() { return isStopFire_; }

	void ChangeStatePattern(std::unique_ptr<BaseBossState> playerState);

	bool IsMoveSucces() { return isMoveSucces_; }


	void BeforeActionMosion();

	bool IsMosionFinish() { return isMosionFinish_; }

	void ResetMosionFinish() { isMosionFinish_ = false; }

private:

	std::unique_ptr<Object_glTF> object_;
	WorldTransform wt_;

	AABB aabb_;

	std::list<std::shared_ptr<EnemyBullet>> bullets_;

	Vector3 kCenter_ = { 0,0,0 };

	const Vector3 kHazi_ = { 20,0,0 };

	Segment move_{};
	float bunkatu_ = 180.0f;//分割

	//攻撃カウント
	uint32_t attackCount_ = 0;

	//体力
	const uint32_t maxHp_ = 60;
	uint32_t hp_ = maxHp_;

	bool isMoveSucces_ = false;

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
	//最大弾丸数
	uint32_t rapidCountMax_ = 10;

	//モーション終了処理
	bool isMosionFinish_ = false;
	const float kRotationX_ = 10.0f;

	//ステートパターン
	std::unique_ptr<BaseBossState> bossState_;
};
