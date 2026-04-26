#pragma once
#include <memory>
#include <vector>

#include "Audio.h"

#include "BaseUmbrella.h"
#include "Umbrella.h"
#include "FoldingUmbrella.h"

class Player;

class BaseCommand {
public:

	void SetPlayer(Player* player) { player_ = player; }
	/// <summary>
	/// 出力処理
	/// </summary>
	virtual void Execute() = 0;

protected:
	Player* player_ = nullptr;
};

class MoveCommand : public BaseCommand{
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	void Execute() override;
private:

	/// <summary>
	/// 移動A
	/// </summary>
	void CommandMoveA();
	/// <summary>
	/// 移動D
	/// </summary>
	void CommandMoveD();
	/// <summary>
	/// 移動W
	/// </summary>
	void CommandMoveW();
	/// <summary>
	/// 移動S
	/// </summary>
	void CommandMoveS();

	/// <summary>
	/// 移動速度の調整
	/// </summary>
	void SpeedParameter();

	/// <summary>
	/// 移動
	/// </summary>
	/// <param name="speed">移動速度</param>
	/// <param name="playerDirection">プレイヤーの現在の向き</param>
	void MovePlayer(float speed,float playerDirection);

	/// <summary>
	/// 傘の8方向の回転
	/// </summary>
	/// <param name="direction">回転角度</param>
	void UmbrellaRange(float direction);


	const float kStickPower_ = 0.5f;//スティックの倒し具合

	//向き
	const float kUpDis_ = 270.0f;//上
	const float kDownDis_ = 90.0f;//下
	const float kLeftDis_ = 180.0f;//左
	const float kRightDis_ = 360.0f;//右

	const float kDiagonalValue_ = 45.0f;//斜めにする変数
	const float kPlayerFrontRange_ = 180.0f;//プレイヤーがカメラから見て正面を向く

	const float kDirectionRight_ = 90.0f;
	const float kDirectionLeft_ = -90.0f;

	const float kMaxAngle_ = 360.0f;

	//プレイヤーの速さ
	const float kStandardSpeed_ = 0.14f;//通常の速さ
	float speed_ = kStandardSpeed_;


	float prevDirectionWidth_ = 0.0f;//横の向き

};

class JumpCommand : public BaseCommand {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	void Execute() override;
private:
	//ジャンプフラグ
	float jumpPower_ = 0.0f;//上がる高さ
	const float kJumpPowerMax_ = 0.3f;//上がる高さ
};

class FireCommand : public BaseCommand {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	void Execute() override;

private:
};

class ShieldCommand: public BaseCommand{
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	void Execute() override;

private:
	/// <summary>
	/// 滑空処理
	/// </summary>
	void Gliding();

	const float kUpDis_ = 270.0f;//上

};

class BrinkCommand : public BaseCommand {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	void Execute() override;

private:
	const float kLeftDis_ = 180.0f;//左

	//傘の位置設定時に使う
	const float kBrinkPower_ = 1.25f;

	const float kBrinkTimeMax_ = 0.5f;//最大値

};

class WeaponChangeCommand : public BaseCommand {
public:
	/// <summary>
	/// 出力処理
	/// </summary>
	void Execute() override;
private:
	/// <summary>
	/// 切り替え右方向
	/// </summary>
	void RightSwitching();
	/// <summary>
	/// 切り替え左方向
	/// </summary>
	void LeftSwitching();

	int weaponNum_ = 0;
	int prevWeaponNum_ = 0;

	enum Weapon {
		shotgun,
		sniper,
		Max
	};

	const int kWeaponMax_ = Weapon::Max - 1;

	std::unordered_map<int, std::function<std::unique_ptr<BaseUmbrella>()>> weaponUmbrella_ = {
		{ Weapon::shotgun, [](){ return std::make_unique<Umbrella>(); } },
		{ Weapon::sniper, [](){ return std::make_unique<FoldingUmbrella>(); } }
	};

};

class PlayerActionsInputHandler {
public:
	/// <summary>
	/// getter_コマンド
	/// </summary>
	/// <returns>入力によって出されたコマンド</returns>
	void GetCommand(Player* player ,std::vector<std::unique_ptr<BaseCommand>>& commands);
private:
	//入力中処理
	bool isInput_ = false;


	/// <summary>
	/// 切り替え右方向
	/// </summary>
	void RightSwitching();
	/// <summary>
	/// 切り替え左方向
	/// </summary>
	void LeftSwitching();

	int weaponNum_ = 0;
	int prevWeaponNum_ = 0;

	enum Weapon {
		shotgun,
		sniper
	};
};
