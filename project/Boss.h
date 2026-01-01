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

	void SetTransform(const Transform& translate) { 
		wt_.translation_ = translate.translate;
		wt_.rotation_ = translate.rotate;
		wt_.scale_ = translate.scale;
	}

	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// setter_当たり判定
	/// </summary>
	/// <param name="aabb">AABB</param>
	void SetAABB(const AABB& aabb) { aabb_ = aabb; }

	
	/// <summary>
	//右に移動
	/// </summary>
	void MoveRight();
	/// <summary>
	/// 左に移動
	/// </summary>
	void MoveLeft();

	uint32_t MoveCount() { return moveCount_; }

	void AddMoveCount() { moveCount_++; }

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

private:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="isArrived"></param>
	void ArrivedSegmentDiff(bool isArrived);

	std::unique_ptr<Object_glTF> object_;
	WorldTransform wt_;

	AABB aabb_;

	std::list<std::unique_ptr<EnemyBullet>> bullets_;

	Vector3 kCenter_ = { 0,0,0 };

	const Vector3 kHazi_ = { 20,0,0 };

	Segment move_{};
	const float kBunkatu = 180.0f;
	//Vector3 nextMove_ = { 0,0,0 };
	
	//動いたカウント
	uint32_t moveCount_ = 0;


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

	std::unique_ptr<BaseBossState> bossState_;
};
