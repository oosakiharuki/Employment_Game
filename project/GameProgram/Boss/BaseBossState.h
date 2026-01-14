#pragma once
#include "MyMath.h"

class Boss;
/// <summary>
/// ボスの基盤ステート
/// </summary>
class BaseBossState{
public:
	virtual void Update(Boss& boss);
};

/// <summary>
/// ボスの横移動ステート
/// </summary>
class BossMoveState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	uint32_t addCount_ = 0;
	//端(中心から左端+,右端-)
	const Vector3 kEdge_ = { 20,0,0 };
};

/// <summary>
/// ボスの攻撃ステート
/// </summary>
class BossAttackState : public BaseBossState {
public:
	void Update(Boss& boss) override;
};

/// <summary>
/// ボスの周り移動ステート
/// </summary>
class BossAroundMoveState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	uint32_t aroundMoveCount_ = 0;

	struct MovePoint {
		Vector3 position;
		float division;
	};

	std::vector<MovePoint> movePoints_;
};

/// <summary>
/// ボスの行動前モーションステート
/// </summary>
class BossBeforeActionMotionState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	const uint32_t kMoveCountMax_ = 2;
	const uint32_t kDeepAttackCountMax_ = 3;

	const float kMoveCoolTimeMax_ = 0.5f;
	float moveCoolTimer_ = 0.0f;
};

/// <summary>
/// 奥行きのある攻撃
/// </summary>
class BossFarAttackState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	const Vector3 kFarPlace_ = { 0,12,30 };

	Vector3 position_{};

	bool isStart = true;

	bool isAttackFinish_ = false;
};

class BossFarTackleState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	bool isStart_ = true;
	Vector3 playerPosition_{};

	const float kMoveFrame_ = 120.0f;

	const float kNearEnd = -48.0f;
};

class BossFallPlayerState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	/// <summary>
	/// 移動ポイントの設定
	/// </summary>
	/// <param name="boss">ボスのクラス</param>
	void MovePointSetting(Boss& boss);

	bool isMovingStartPoint = true;

	float fallTimer_ = 0.0f;

	const float kFallTimeMax_ = 3.0f;//3秒後に落ちる
	const float kFallPointY_ = 3.0f;//落ちる上限

	const float kGoUpTime_ = kFallTimeMax_ + 2.0f;//上に上がる時間
	const float kGoUpPointY_ = 13.0f;//上げる位置

	//高い場所
	const Vector3 kStartPoint_ = { 0,25,0 };

	Vector3 movePoint_{};
	float moveFrame_ = 180.0f;

	const float kFallFrame_ = (kGoUpTime_ - kFallTimeMax_ - 1.0f) * 60.0f;


	bool isFinish_ = false;
};

class BossDeadMotionState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	bool isStart_ = true;
};
