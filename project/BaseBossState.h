#pragma once
#include <cstdint>
#include <vector>
#include "Vector3.h"

class Boss;
/// <summary>
/// ボスの基盤ステート
/// </summary>
class BaseBossState {
public:
	virtual void Update(Boss& boss);
};

/// <summary>
/// ボスの横移動ステート
/// </summary>
class BossMoveState : public BaseBossState {
public:
	void Update(Boss& boss) override;
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
};

