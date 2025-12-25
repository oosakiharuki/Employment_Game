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
	virtual void Update(Player& player);
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
