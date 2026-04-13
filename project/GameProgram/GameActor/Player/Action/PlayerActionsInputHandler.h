#pragma once
#include <memory>

class PlayerActions;

class Command {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	virtual void Execute(PlayerActions* player) = 0;
};

class MoveCommand : public Command{
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerActions* player) override;
private:
	const float kStickPower_ = 0.5f;//スティックの倒し具合
};

class JumpCommand : public Command {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerActions* player) override;
private:
	std::unique_ptr<MoveCommand> moveCommand_;
};

class FireCommand : public Command {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerActions* player) override;
private:
	std::unique_ptr<MoveCommand> moveCommand_;
};

class ShieldCommand: public Command{
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerActions* player) override;
private:
	std::unique_ptr<MoveCommand> moveCommand_;
};

class BrinkCommand : public Command {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerActions* player) override;
};


class PlayerActionsInputHandler {
public:
	/// <summary>
	/// getter_コマンド
	/// </summary>
	/// <returns>入力によって出されたコマンド</returns>
	std::unique_ptr<Command> GetCommand();
	/// <summary>
	/// setter_プレイヤーコマンド
	/// </summary>
	/// <param name="PlayerActions"></param>
	void SetPlayerActions(PlayerActions* playerActions) { playerActions_ = playerActions; }
private:

	PlayerActions* playerActions_ = nullptr;

	//入力中処理
	bool isInput_ = false;
};
