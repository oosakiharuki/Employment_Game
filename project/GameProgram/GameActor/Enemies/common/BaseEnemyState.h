#pragma once
#include <memory>

class BaseEnemy;

/// <summary>
/// 敵の行動パターン(基盤クラス)
/// </summary>
class BaseEnemyState {
public:
	virtual void Update(BaseEnemy& enemy) = 0;
	virtual void ChangeState(BaseEnemy& enemy) = 0;
};

/// <summary>
/// 敵の索敵ステート
/// </summary>
class EnemySearchState : public BaseEnemyState {
public:
	void Update(BaseEnemy& enemy) override;
	void ChangeState(BaseEnemy& enemy) override;
};

/// <summary>
/// 敵の攻撃ステート
/// </summary>
class EnemyAttackState : public BaseEnemyState {
public:
	void Update(BaseEnemy& enemy) override;
	void ChangeState(BaseEnemy& enemy) override;
};
