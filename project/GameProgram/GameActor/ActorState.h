#pragma once
#include "MyMath.h"
#include <memory>

class GameActor;

/// <summary>
/// ゲームアクターの状態ステート
/// </summary>
class ActorCommand {
public:
	/// <summary>
	/// 生存状態
	/// </summary>
	virtual void Active() = 0;
	/// <summary>
	/// 死亡状態
	/// </summary>
	virtual void Dead() = 0;
	/// <summary>
	/// 演出状態
	/// </summary>
	virtual void Performance() = 0;
};

/// <summary>
/// ゲームアクター状態ステート(基盤クラス)
/// </summary>
class BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="command">ゲームアクターのコマンドクラス</param>
	virtual void Update(ActorCommand& command) = 0;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="actor">ゲームアクタークラス</param>
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
	/// <param name="command">ゲームアクターのコマンドクラス</param>
	void Update(ActorCommand& actorCommand) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="actor">ゲームアクタークラス</param>
	void ChangeState(GameActor& actor) override;
};

class DeadState : public BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="command">ゲームアクターのコマンドクラス</param>
	void Update(ActorCommand& actorCommand) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="actor">ゲームアクタークラス</param>
	void ChangeState(GameActor& actor) override;
};


class PerformanceState : public BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="command">ゲームアクターのコマンドクラス</param>
	void Update(ActorCommand& actorCommand) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="actor">ゲームアクタークラス</param>
	void ChangeState(GameActor& actor) override;
};
