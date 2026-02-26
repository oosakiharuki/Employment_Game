#pragma once
#include "MyMath.h"
#include <memory>

class Boss;

class BossCommand {
public:
	virtual void CommandMove() = 0;
	virtual void CommandFire(float kFrame, float bulletSpeed, uint32_t bulletMax) = 0;
	virtual void CommandAroundMove() = 0;
	virtual void CommandFarMove() = 0;
	virtual void CommandFarTackle() = 0;
	virtual void CommandFallPlayer() = 0;
	virtual void CommandBeforeActionMotion() = 0;

	/// <summary>
	/// 行動カウント
	/// </summary>
	void AddActionCount() { actionCount_++; }
	/// <summary>
	/// 行動カウントリセット
	/// </summary>
	void ResetActionCount() { actionCount_ = 0; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool FarMoveSuccess();

protected:

	float bulletSpeed_ = 0.2f;//弾の速さ

	Vector3 moveCenter_ = { 0,0,0 };

	Segment move_{};
	float moveTimer_ = 0.0f;
	float timerMax_ = 1.0f;//到着時間

	uint32_t addCount_ = 0;
	//端(中心から左端+,右端-)
	const Vector3 kEdge_ = { 15,0,0 };

	//攻撃カウント
	uint32_t actionCount_ = 0;

	//攻撃

	//弾丸の出す間の時間
	float rapidFireTime_ = 0.0f;
	const float kRapidFireTimeMax_ = 0.1f;
	//発射カウント
	uint32_t rapidCount_ = 0;

	//行動前モーション終了処理
	const float kRotateOneLap_ = 360.0f; //回転
	const float kRotationX_ = 10.0f;     //回転速度
	float moveCoolTimer_ = 0.0f;         //行動映るクールタイム
	const float kMoveCoolTimeMax_ = 0.5f;//次の行動に移る時間

	//構造体_線形用移動
	struct MovePoint {
		Vector3 position;//位置
		float division;  //到達時間
	};
	std::vector<MovePoint> movePoints_;

	uint32_t aroundMoveCount_ = 0;


	const Vector3 kFarPlace_ = { 0,12,30 };
	bool isFarMoveSuccess_ = false;

	const float kNearEnd = -48.0f;

	//fallPlayer
	float fallTimer_ = 0.0f;

	const float kPrepareFallTimeMax_ = 3.0f;//3秒後に落ちる
	const float kFallTimeMax_ = 1.5f;//落ちる速度(1.5秒)

	const float kFallPointY_ = 3.0f;//落ちる上限

	const float kGoUpTime_ = kPrepareFallTimeMax_ + kFallTimeMax_ + 2.0f;//上に上がる時間
	const float kGoUpPointY_ = 13.0f;//上げる位置

	//高い場所
	const float kStartPointY_ = 25.0f;

	Vector3 movePoint_{};
	float moveFrame_ = 3.0f;
};


/// <summary>
/// ボスの基盤ステート
/// </summary>
class BaseBossState {
public:
	virtual void Update(BossCommand& bossCommand) = 0;
	virtual void ChangeCommand(Boss& boss) = 0;

	/// <summary>
	/// ステートパターン変更フラグ
	/// </summary>
	/// <returns>次のステートの値が入っている場合</returns>
	bool GetIsInput() {
		if (nextState_ != nullptr) {
			return true;
		}
		return false;
	}
	/// <summary>
	/// getter_次のステートパターン
	/// </summary>
	/// <returns></returns>
	std::unique_ptr<BaseBossState> GetNextState() { return std::move(nextState_); }

protected:
	//次に変更するステートの入れ物
	std::unique_ptr<BaseBossState> nextState_;
};

/// <summary>
/// ボスの横移動ステート
/// </summary>
class BossMoveState : public BaseBossState {
public:
	void Update(BossCommand& bossCommand) override;
	void ChangeCommand(Boss& boss) override;
private:

};

/// <summary>
/// ボスの攻撃ステート
/// </summary>
class BossAttackState : public BaseBossState {
public:
	void Update(BossCommand& bossCommand) override;
	void ChangeCommand(Boss& boss) override;
private:
	//秒数
	const float kTimerFrame_ = 2.0f;
	//弾丸の速さ
	const float kBulletSpeed_ = 0.2f;
	//最大弾丸数
	const uint32_t kRapidCountMax_ = 5;
};

/// <summary>
/// ボスの周り移動ステート
/// </summary>
class BossAroundMoveState : public BaseBossState {
public:
	void Update(BossCommand& bossCommand) override;
	void ChangeCommand(Boss& boss) override;
private:
	//uint32_t aroundMoveCount_ = 0;

	//struct MovePoint {
	//	Vector3 position;
	//	float division;
	//};

	//std::vector<MovePoint> movePoints_;
};

/// <summary>
/// ボスの行動前モーションステート
/// </summary>
class BossBeforeActionMotionState : public BaseBossState {
public:
	void Update(BossCommand& bossCommand) override;
	void ChangeCommand(Boss& boss) override;
private:
	const uint32_t kMoveCountMax_ = 2;
	const uint32_t kDeepAttackCountMax_ = 3;



};

/// <summary>
/// 奥行きのある攻撃
/// </summary>
class BossFarAttackState : public BaseBossState {
public:
	void Update(BossCommand& bossCommand) override;
	void ChangeCommand(Boss& boss) override;
private:
	//秒数
	const float kTimerFrame_ = 4.0f;
	//弾丸の速さ
	const float kBulletSpeed_ = 0.4f;
	//最大弾丸数
	const uint32_t kRapidCountMax_ = 10;

	bool isFire_ = false;
};

class BossFarTackleState : public BaseBossState {
public:
	void Update(BossCommand& bossCommand) override;
	void ChangeCommand(Boss& boss) override;
private:
	bool isStart_ = true;
	Vector3 playerPosition_{};

	const float kMoveFrame_ = 120.0f;

};

class BossFallPlayerState : public BaseBossState {
public:
	void Update(BossCommand& bossCommand) override;
	void ChangeCommand(Boss& boss) override;
private:

};
