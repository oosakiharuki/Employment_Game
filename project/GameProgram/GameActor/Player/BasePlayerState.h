#pragma once
#include "MyMath.h"
#include <memory>

class Player;

class PlayerCommand {
public:
	virtual void CommandMove() = 0;
	virtual void CommandJump() = 0;
	virtual void CommandFire() = 0;
	virtual void CommandShield() = 0;
	virtual void CommandBrink() = 0;

	virtual void Active() = 0;
	virtual void Dead() = 0;
	virtual void Performance() = 0;

protected:
	//input
	const float kStickPower_ = 0.5f;//スティックの倒し具合

	//プレイヤーの速さ
	const float kStandardSpeed_ = 0.14f;//通常の速さ
	float speed_ = kStandardSpeed_;
	
	//ジャンプフラグ
	float jumpPower_ = 0.0f;//上がる高さ
	const float kJumpPowerMax_ = 0.3f;//上がる高さ
	
	//ボタン
	bool isPushA_ = false;
	bool isPushD_ = false;
	bool isPushW_ = false;
	bool isPushS_ = false;


	//傘のシールドフラグ
	bool isShield_ = false;
	
	//パリィ
	bool isParry_ = false;
	const float kParryTimeMax_ = 0.5f;//パリィする時間//ちょっと簡単に
	float parryTime_ = kParryTimeMax_;
	const Vector3 kPlayerFront_ = { 0,0,1.5f };//プレイヤーの前方
	const float kBrinkPower_ = 1.25f;

	///ブリンク
	bool isOneBrink_ = false;//一回のみ
	//ブリンク時間
	float brinkTimer_ = 0.0f;
	const float kBrinkTimeMax_ = 0.5f;//最大値
	
};

class BasePlayerState {
public:
	virtual void Update(PlayerCommand& command) = 0;
	virtual void CommandInput(Player& player) = 0;

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
	std::unique_ptr<BasePlayerState> GetNextState() { return std::move(nextState_); }

protected:
	//次に変更するステートの入れ物
	std::unique_ptr<BasePlayerState> nextState_;
};

class PlayerActiveState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(PlayerCommand& playerCommand) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="player">使っているプレイヤー</param>
	void CommandInput(Player& player) override;
};

class PlayerDeadState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(PlayerCommand& playerCommand) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="player">使っているプレイヤー</param>
	void CommandInput(Player& player) override;
};


class PlayerPerformanceState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(PlayerCommand& playerCommand) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="player">使っているプレイヤー</param>
	void CommandInput(Player& player) override;
};


/// <summary>
/// プレイヤー発砲攻撃状態ステート
/// </summary>
class PlayerNormalState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="player">使っているプレイヤー</param>
	void CommandInput(Player& player) override;
};

/// <summary>
/// プレイヤー発砲攻撃状態ステート
/// </summary>
class PlayerJumpState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="player">使っているプレイヤー</param>
	void CommandInput(Player& player) override;
};


/// <summary>
/// プレイヤー発砲攻撃状態ステート
/// </summary>
class PlayerFireState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="player">使っているプレイヤー</param>
	void CommandInput(Player& player) override;
};

/// <summary>
/// プレイヤー防御状態ステート
/// </summary>
class PlayerShieldState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="player">使っているプレイヤー</param>
	void CommandInput(Player& player) override;
};

/// <summary>
/// プレイヤーブリンク状態ステート
/// </summary>
class PlayerBrinkState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// キーでステート変更
	/// </summary>
	/// <param name="player">使っているプレイヤー</param>
	void CommandInput(Player& player) override;
};

