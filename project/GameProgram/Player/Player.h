#pragma once
#include "Object_glTF.h"
#include "PlayerBullet.h"
#include "Umbrella.h"
#include "Audio.h"
#include "Input.h"

#include "GameActor.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player : public GameActor{
public:
	Player();
	~Player();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// パーティクル処理
	/// </summary>
	void DrawP();

	/// <summary>
	/// 操作できるときの処理()
	/// </summary>
	void PlayUpdate();

	/// <summary>
	/// getter_ワールド座標系
	/// </summary>
	/// <returns></returns>プレイヤー本体のワールド座標系
	const WorldTransform& GetWorldTransform() { return wt_; }

	/// <summary>
	/// 弾を発射する(ショットガン風)
	/// </summary>
	void ShootBullet();

	/// <summary>
	/// getter_弾丸リスト
	/// </summary>
	/// <returns></returns>弾丸リスト
	std::list<std::shared_ptr<PlayerBullet>> GetBullets() {
		return bullets_;
	};
	
	/// <summary>
	/// getter_傘
	/// </summary>
	/// <returns></returns>使用している傘クラス
	Umbrella* GetUmbrella() { return umbrella_.get(); }

	/// <summary>
	/// getter_シールドフラグ
	/// </summary>
	/// <returns></returns>trueなら守っている
	bool GetIsShield() { return isShield_; }

	/// <summary>
	/// ダメージを食らった
	/// </summary>
	/// <param name="hitPoint"></param>当たった場所
	void IsDamage(const Vector3& hitPoint);
	/// <summary>
	/// 落下した(即死)
	/// </summary>
	void IsFall();

	/// <summary>
	/// プレイヤーが当たったノックバック
	/// </summary>
	/// <param name="Power"></param>ノックバックの強さ
	/// <param name="TimerMax"></param>ノックバックする時間(EaseInを仕様しているため)
	void KnockBackPlayer(const Vector3 Power, const float TimerMax);
	/// <summary>
	/// 傘が当たったノックバック
	/// </summary>
	/// <param name="Power"></param>ノックバックの強さ
	/// <param name="TimerMax"></param>ノックバックする時間(EaseInを仕様しているため)
	void KnockBackUmbrella(const Vector3 Power,const float TimerMax);

	/// <summary>
	/// 死んだときの処理
	/// </summary>
	void DeadPlayer();

	/// <summary>
	/// getter_復活
	/// </summary>
	/// <returns></returns>復活フラグ
	bool GetIsRespawn() { return isRespawn_; }
	
	/// <summary>
	/// 全ての敵が初期地に戻った時
	/// </summary>	
	void RespawnPlayer();

	/// <summary>
	/// getter_パリィ
	/// </summary>
	/// <returns></returns>跳ね返したフラグ
	bool GetIsPari() { return isPari_; }

	/// <summary>
	/// パリィ成功 = 連続弾も跳ね返す
	/// </summary>
	void PariSuccess();
	/// <summary>
	/// パリィ成功フラグ
	/// </summary>
	/// <param name="result"></param>trueで成功
	void SetIsPari(bool result) { isPari_ = result; }

	/// <summary>
	/// getter_体力
	/// </summary>
	/// <returns></returns>現在の体力
	uint32_t GetHp() { return hp_; }

	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="preHp"></param>代入する体力数
	const uint32_t GetMaxHp() { return kPlayerMaxHp_; }

	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="preHp"></param>代入する体力数
	void SetHp(uint32_t preHp) { hp_ = preHp; }

	/// <summary>
	/// setter_影の位置
	/// </summary>
	/// <param name="position"></param>影の位置の登録
	void SetShadowPosition(Vector3 position) { shadow_->SetTranslate(position); }

	//スプライトの変化
	void SpriteUpdate();

	/// <summary>
	/// 傘が当たったリアクションフラグ
	/// </summary>
	void IsShildMosion() { isShildMosion_ = true; }

	/// <summary>
	/// 強制的にジャンプさせる(演出等で使う)
	/// </summary>
	void IsJumping() { isJump_ = true; }

	/// <summary>
	/// 傘の8方向の回転
	/// </summary>
	/// <param name="direction"></param>回転角度
	void UmbrellaRange(const float& direction);

	const bool Performancing() { return isPerformance_; }

private:
	//オブジェクト
	std::unique_ptr<Object_glTF> object_;

	//input
	Input* input_ = nullptr;
	XINPUT_STATE state_, preState_;
	//プレイヤーの速さ
	const float kStandardSpeed_ = 0.14f;
	float speed_ = kStandardSpeed_;
	//ジャンプフラグ
	bool isJump_ = false;

	//重力
	void GravityUpdate();
	const float kStandardGrabity_ = 0.01f;//重力の質量
	const float kFixedGrabity_ = -0.05f;//滑空時重力値を固定

	/// 弾丸
	std::list<std::shared_ptr<PlayerBullet>> bullets_;
	float coolTimer_ = 0.0f;//クールタイム
	const float kCoolTimeMax_ = 0.5f;
	uint32_t bulletCount_ = 3;//一度に出る弾丸数

	//ボタン
	bool isPushA_ = false;
	bool isPushD_ = false;
	bool isPushW_ = false;
	bool isPushS_ = false;
	
	//向き
	const float kUpDis_ = 270.0f;//上
	const float kDowntDis_ = 90.0f;//下
	const float kLeftDis_ = 180.0f;//左
	const float kRightDis_ = 360.0f;//右

	const float kNanameValue_ = 45.0f;//斜めにする変数

	//傘銃
	std::unique_ptr<Umbrella> umbrella_ = nullptr;
	WorldTransform wtGun_;//傘のワールド座標系

	//傘のシールドフラグ
	bool isShield_ = false;
	//パリィ
	bool isPari_ = false;
	const float kPariTimeMax_ = 0.1f;//パリィする時間
	float pariTime_ = kPariTimeMax_;
	float pariCoolTime_ = 0.0f;
	const Vector3 playerFront_ = { 0,0,1.5f };//プレイヤーの前方

	/// ノックバック
	bool isKnockback_ = false;
	Vector3 backPower_ = { 0,0,0 };
	float knockBackTimer_ = 0.0f;
	float knockBackTimeMax_ = 0.5f;

	///ブリンク
	bool isBrink_ = false;
	bool isOneBrink_ = false;
	float brinkTimer_ = 0.0f;
	const float kBrinkTimeMax_ = 0.5f;

	//落下する時ふわふわできるように
	bool isUmbrellaFall_ = false;

	const uint32_t kPlayerMaxHp_ = 3;//設定する体力
	//ダメージを食らった後の無敵時間
	float infinityTimer_ = 0.0f;
	const float kInfinityTimeMax_ = 1.0f;

	//倒された
	float deadTimer_ = 0.0f;
	const float kHitStopTime_ = 1.0f;
	const float kDeadTimeMax_ = 3.0f;

	//復活
	bool isRespawn_ = false;

	//サウンド
	SoundData hitSound_;//ダメージを食らった
	SoundData pariSound_;//パリィに成功
	const float kVolume_ = 0.07f;

	//パーティクル
	std::unique_ptr<Particle> particleWalk_;//歩く
	std::unique_ptr<Particle> particleFire_;//撃つ
	std::unique_ptr<Particle> particleBrink_;//ブリンク
	std::unique_ptr<Particle> particleDamage_;//ダメージを食らった
	std::unique_ptr<Particle> particlePari_;//パリィ成功
	std::unique_ptr<Particle> particleDead_;//倒された演出

	//前回座標の保存場所
	Vector3 PrePosition;

	//オブジェクトたち
	std::unordered_map<std::string, std::string> objectMosions_;

	//アニメーション変更
	std::string mosionName_ = "standby";
	std::string preMosionName_ = "standby"; 

	//UI
	std::vector<std::unique_ptr<Sprite>> spritesHp_;
	const Vector2 kTextureSizeHp_ = { 64,64 };//スプライトサイズ
	const Vector2 kInitializePointHp_ = { 20.0f,45.0f };//スプライトの初期位置
	const float kDistanceYHp_ = 10.0f;//スプライトのY軸幅

	//傘がリアクションするflag
	bool isShildMosion_ = false;
};