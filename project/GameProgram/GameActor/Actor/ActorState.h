#pragma once
#include "MyMath.h"
#include <memory>

class GameActor;

/// <summary>
/// ゲームアクター状態ステート(基盤クラス)
/// </summary>
class BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="actor">ゲームアクターのクラス</param>
	virtual void Update(GameActor& actor) = 0;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="actor">ゲームアクタークラス</param>
	virtual void ChangeState(GameActor& actor) = 0;
};

/// <summary>
/// 生存状態ステート
/// </summary>
class ActiveState : public BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="actor">ゲームアクターのコマンドクラス</param>
	void Update(GameActor& actor) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="actor">ゲームアクタークラス</param>
	void ChangeState(GameActor& actor) override;
};

/// <summary>
/// 死亡状態ステート
/// </summary>
class DeadState : public BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="actor">ゲームアクターのコマンドクラス</param>
	void Update(GameActor& actor) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="actor">ゲームアクタークラス</param>
	void ChangeState(GameActor& actor) override;
};

/// <summary>
/// 演出状態ステート
/// </summary>
class PerformanceState : public BaseActorState {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="actor">ゲームアクターのコマンドクラス</param>
	void Update(GameActor& actor) override;
	/// <summary>
	/// ステート変更処理
	/// </summary>
	/// <param name="actor">ゲームアクタークラス</param>
	void ChangeState(GameActor& actor) override;
};
