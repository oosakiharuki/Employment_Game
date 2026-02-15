#pragma once
#include "MyMath.h"
#include <memory>

class GameActor;

class ActorCommand {
public:
	virtual void Active() = 0;
	virtual void Dead() = 0;
	virtual void Performance() = 0;
};


class BaseActorState {
public:
	virtual void Update(ActorCommand& command) = 0;
	virtual void ChangeState(GameActor& actor) = 0;

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
	std::unique_ptr<BaseActorState> GetNextState() { return std::move(nextState_); }

protected:
	//次に変更するステートの入れ物
	std::unique_ptr<BaseActorState> nextState_;
};

class ActiveState : public BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(ActorCommand& actorCommand) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="actor">使用しているゲームアクター</param>
	void ChangeState(GameActor& actor) override;
};

class DeadState : public BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(ActorCommand& ActorCommand) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="actor">使用しているゲームアクター</param>
	void ChangeState(GameActor& actor) override;
};


class PerformanceState : public BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(ActorCommand& ActorCommand) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="actor">使用しているゲームアクター</param>
	void ChangeState(GameActor& actor) override;
};
