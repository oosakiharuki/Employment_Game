#pragma once
#include "MyMath.h"
#include "PlayerBullet.h"
#include <memory>
#include <list>

#include "Audio.h"

class Player;
/// <summary>
/// プレイヤーの行動
/// </summary>
class PlayerActions {
public:
	/// <summary>
	/// setter_プレイヤー
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void SetPlayer(Player* player) { player_ = player; }

	void InitAudio();

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

	void MovePlayer(float speed,float playerDirection);

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
	/// 弾を発射する(ショットガン風)強い
	/// </summary>
	void PowerShootBullet();

	/// <summary>
	/// 弾の生成
	/// </summary>
	/// <param name="translate">傘から出るため</param>
	/// <param name="velocity">弾丸速度</param>
	void BornBullet(const Vector3& translate, const Vector3& velocity);

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

	//プレイヤーの速さ
	const float kStandardSpeed_ = 0.14f;//通常の速さ
	float speed_ = kStandardSpeed_;
	
	//ジャンプフラグ
	float jumpPower_ = 0.0f;//上がる高さ
	const float kJumpPowerMax_ = 0.3f;//上がる高さ

	float prevDirectionWidth_ = 0.0f;//横の向き

	/// 弾丸
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	const float kDispersionBetween_ = 0.1f;//分散する間
	const float kBulletSpeed_ = 0.5f;//弾丸の前方向の速さ

	float fireCoolTimer_ = 0.0f;//クールタイマー
	const float kFireCoolTimeMax_ = 0.5f;//クールタイム最大時間
	const uint32_t kBulletCount_ = 3;//一度に出る弾丸数
	
	//真ん中を0にする値(3の場合、1,0,-1 | 5の場合、2,1,0,-1,-2)
	float halfCount = float((kBulletCount_ - 1) * 0.5f);//二で割る

	const Vector3 kBulletKnockbackPower_ = { 0.0f,0.0f,0.1f };//撃った場合のノックバックパワー

	const uint32_t kPowerBulletCount_ = 2;
	bool isCount_ = false;//一回のみ


	//傘の位置設定時に使う
	const float kBrinkPower_ = 1.25f;

	///ブリンク
	bool isOneBrink_ = false;//一回のみ
	//ブリンク時間
	float brinkTimer_ = 0.0f;
	const float kBrinkTimeMax_ = 0.5f;//最大値
	

	//サウンド
	std::unique_ptr<SoundData> fireSound_;//発砲攻撃
	std::unique_ptr<SoundData> jumpSound_;//ジャンプ
	const float kVolume_ = 0.3f;//ボリューム

	Player* player_ = nullptr;
};
