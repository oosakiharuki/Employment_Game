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
	//端
	const Vector3 kHazi_ = { 20,0,0 };
	//動きの分割数
	const float kMoveFrame_ = 180.0f;

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
		float bunkatu;
	};

	std::vector<MovePoint> movePoints_;
};

/// <summary>
/// ボスの行動前モーションステート
/// </summary>
class BossBeforeActionMosionState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	const uint32_t kMoveCountMax_ = 2;
	const uint32_t kDeepAttackCountMax_ = 3;

	const float kMoveCoolTimeMax_ = 0.5f;
	float moveCoolTimer_ = 0.0f;
};

class BossFarAttackState : public BaseBossState {
public:
	void Update(Boss& boss) override;
private:
	const Vector3 kFarPlace_ = { 0,12,30 };
	const float kBunkatu_ = 180.0f;

	Vector3 position_{};

	bool isStart = true;

	bool isAttackFinish_ = false;
};