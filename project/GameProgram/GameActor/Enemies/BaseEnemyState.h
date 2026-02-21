#pragma once
#include <memory>

class BaseEnemy;

class EnemyCommand {
public:
	virtual void SearchCommand() = 0;
	virtual void AttackCommand() = 0;
};

class BaseEnemyState {
public:
	virtual void Update(EnemyCommand& enemy) = 0;
	virtual void ChangeState(BaseEnemy& enemy) = 0;

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
	std::unique_ptr<BaseEnemyState> GetNextState() { return std::move(nextState_); }

protected:
	//次に変更するステートの入れ物
	std::unique_ptr<BaseEnemyState> nextState_;
};

class EnemySearchState : public BaseEnemyState {
public:
	void Update(EnemyCommand& enemy) override;
	void ChangeState(BaseEnemy& enemy) override;
};

class EnemyAttackState : public BaseEnemyState {
public:
	void Update(EnemyCommand& enemy) override;
	void ChangeState(BaseEnemy& enemy) override;
};
