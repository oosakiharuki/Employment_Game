#pragma once
#include "MyMath.h"
#include <memory>

class Player;
/// <summary>
/// プレイヤーの行動コマンド
/// </summary>
class PlayerCommand {
public:
	/// <summary>
	/// 移動
	/// </summary>
	virtual void CommandMove() = 0;
	/// <summary>
	/// ジャンプ
	/// </summary>
	virtual void CommandJump() = 0;
	/// <summary>
	/// 発砲攻撃
	/// </summary>
	virtual void CommandFire() = 0;
	/// <summary>
	/// 傘シールド
	/// </summary>
	virtual void CommandShield() = 0;
	/// <summary>
	/// ブリンク
	/// </summary>
	virtual void CommandBrink() = 0;

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

	//傘の位置設定時に使う
	const Vector3 kPlayerFront_ = { 0,0,1.5f };//プレイヤーの前方
	const float kBrinkPower_ = 1.25f;

	///ブリンク
	bool isOneBrink_ = false;//一回のみ
	//ブリンク時間
	float brinkTimer_ = 0.0f;
	const float kBrinkTimeMax_ = 0.5f;//最大値
	
};

/// <summary>
/// プレイヤー基盤ステート
/// </summary>
class BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="command">プレイヤーコマンドクラス</param>
	virtual void Update(PlayerCommand& command) = 0;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
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

/// <summary>
/// プレイヤー発砲攻撃状態ステート
/// </summary>
class PlayerNormalState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="command">プレイヤーコマンドクラス</param>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
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
	/// <param name="command">プレイヤーコマンドクラス</param>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
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
	/// <param name="command">プレイヤーコマンドクラス</param>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
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
	/// <param name="command">プレイヤーコマンドクラス</param>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
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
	/// <param name="command">プレイヤーコマンドクラス</param>
	void Update(PlayerCommand& playerCommond) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void CommandInput(Player& player) override;
};

