#pragma once
#include <memory>

class PlayerCommand;

class Command {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	virtual void Execute(PlayerCommand* player) = 0;
};

class MoveCommand : public Command{
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerCommand* player) override;
private:
	const float kStickPower_ = 0.5f;//スティックの倒し具合
};

class JumpCommand : public Command {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerCommand* player) override;
private:
	std::unique_ptr<MoveCommand> moveCommand_;
};

class FireCommand : public Command {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerCommand* player) override;
private:
	std::unique_ptr<MoveCommand> moveCommand_;
};

class ShieldCommand: public Command{
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerCommand* player) override;
private:
	std::unique_ptr<MoveCommand> moveCommand_;
};

class BrinkCommand : public Command {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void Execute(PlayerCommand* player) override;
};


class PlayerActionCommand {
public:
	/// <summary>
	/// getter_コマンド
	/// </summary>
	/// <returns>入力によって出されたコマンド</returns>
	std::unique_ptr<Command> GetCommand();
	/// <summary>
	/// setter_プレイヤーコマンド
	/// </summary>
	/// <param name="playerCommand"></param>
	void SetPlayerCommand(PlayerCommand* playerCommand) { playerCommand_ = playerCommand; }
private:

	PlayerCommand* playerCommand_ = nullptr;
};
