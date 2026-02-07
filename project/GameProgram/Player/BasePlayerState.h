#pragma once

class Player;

/// <summary>
/// プレイヤーのステートパターン(基盤)
/// </summary>
class BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update(Player& player) = 0;
};

/// <summary>
/// プレイヤー通常状態ステート
/// </summary>
class PlayerLifeState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player& player) override;
};

/// <summary>
/// プレイヤー死亡ステート
/// </summary>
class PlayerDeadState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player& player) override;
};

/// <summary>
/// プレイヤー演出ステート
/// </summary>
class PlayerPerformanceState : public BasePlayerState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player& player) override;
};


/// <summary>
/// プレイヤーのステートパターン(基盤)
/// </summary>
class BasePlayerActionState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update(Player& player) = 0;
};

/// <summary>
/// プレイヤー発砲攻撃状態ステート
/// </summary>
class PlayerNormalState : public BasePlayerActionState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player& player) override;
};

/// <summary>
/// プレイヤー発砲攻撃状態ステート
/// </summary>
class PlayerJumpState : public BasePlayerActionState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player& player) override;
};


/// <summary>
/// プレイヤー発砲攻撃状態ステート
/// </summary>
class PlayerFireState : public BasePlayerActionState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player& player) override;
};

/// <summary>
/// プレイヤー防御状態ステート
/// </summary>
class PlayerShieldState : public BasePlayerActionState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player& player) override;
};

/// <summary>
/// プレイヤーブリンク状態ステート
/// </summary>
class PlayerBrinkState : public BasePlayerActionState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player& player) override;
private:
	float brinkTimer_ = 0.0f;
	const float kBrinkTimeMax_ = 0.5f;
};

