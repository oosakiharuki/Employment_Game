#pragma once
#include "MyMath.h"
#include "PlayerBullet.h"
#include <memory>
#include <list>

class Player;
/// <summary>
/// プレイヤーの行動コマンド
/// </summary>
class PlayerCommand {
public:
	/// <summary>
	/// setter_プレイヤー
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 弾丸の更新処理
	/// </summary>
	void BulletUpdate();

	/// <summary>
	/// 弾丸の描画処理
	/// </summary>
	void BulletDraw();

	/// <summary>
	/// ジャンプの更新処理
	/// </summary>
	void JumpUpdate();

	/// <summary>
	/// 移動
	/// </summary>
	void CommandMove();
	/// <summary>
	/// ジャンプ
	/// </summary>
	void CommandJump();
	/// <summary>
	/// 発砲攻撃
	/// </summary>
	void CommandFire();
	/// <summary>
	/// 傘シールド
	/// </summary>
	void CommandShield();
	/// <summary>
	/// ブリンク
	/// </summary>
	void CommandBrink();
	/// <summary>
	///	ブリンク発動条件
	/// </summary>
	/// <returns>発動している</returns>
	bool BrinkFlag();

	/// <summary>
	/// ブリンクタイマー
	/// </summary>
	/// <returns>ブリンクタイマーがMaxに到達した時true</returns>
	bool BrinkTimeMax();
	/// <summary>
	/// シールド解除
	/// </summary>
	void OffShield();

private:

	/// <summary>
	/// 傘の8方向の回転
	/// </summary>
	/// <param name="direction">回転角度</param>
	void UmbrellaRange(float direction);

	/// <summary>
	/// 弾を発射する(ショットガン風)
	/// </summary>
	void ShootBullet();

	/// <summary>
	/// 滑空処理
	/// </summary>
	void Gliding();

	//向き
	const float kUpDis_ = 270.0f;//上
	const float kDownDis_ = 90.0f;//下
	const float kLeftDis_ = 180.0f;//左
	const float kRightDis_ = 360.0f;//右

	const float kDiagonalValue_ = 45.0f;//斜めにする変数
	const float kPlayerFrontRange_ = 180.0f;//プレイヤーがカメラから見て正面を向く

	const float kDirectionRight_ = 90.0f;
	const float kDirectionLeft_ = -90.0f;
	
	//最大角度(360度)
	const float kMaxAngle = 360.0f;

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


	/// 弾丸
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	const float kDispersionBetween_ = 0.1f;//分散する間
	const float kBulletSpeed_ = 0.5f;//弾丸の前方向の速さ

	float fireCoolTimer_ = 0.0f;//クールタイマー
	const float kFireCoolTimeMax_ = 0.5f;//クールタイム最大時間
	const uint32_t kBulletCount_ = 3;//一度に出る弾丸数
	
	const Vector3 kBulletKnockbackPower_ = { 0.0f,0.0f,0.1f };//撃った場合のノックバックパワー

	//傘の位置設定時に使う
	const float kBrinkPower_ = 1.25f;

	///ブリンク
	bool isOneBrink_ = false;//一回のみ
	//ブリンク時間
	float brinkTimer_ = 0.0f;
	const float kBrinkTimeMax_ = 0.5f;//最大値
	
	Player* player_ = nullptr;
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
	virtual void CommandInput(PlayerCommand& command) = 0;

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
	void CommandInput(PlayerCommand& command) override;
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
	void CommandInput(PlayerCommand& command) override;
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
	void CommandInput(PlayerCommand& command) override;
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
	void CommandInput(PlayerCommand& command) override;
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
	void CommandInput(PlayerCommand& command) override;
};

