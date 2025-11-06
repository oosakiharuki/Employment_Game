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
	const WorldTransform& GetWorldTransform() { return wt; }

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
	Umbrella* GetUmbrella() { return umbrella.get(); }

	/// <summary>
	/// getter_シールドフラグ
	/// </summary>
	/// <returns></returns>trueなら守っている
	bool GetIsShield() { return isShield; }

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
	bool GetIsRespawn() { return isRespawn; }
	
	/// <summary>
	/// 全ての敵が初期地に戻った時
	/// </summary>	
	void RespawnPlayer();

	/// <summary>
	/// getter_パリィ
	/// </summary>
	/// <returns></returns>跳ね返したフラグ
	bool GetIsPari() { return isPari; }

	/// <summary>
	/// パリィ成功 = 連続弾も跳ね返す
	/// </summary>
	void PariSuccess();
	/// <summary>
	/// パリィ成功フラグ
	/// </summary>
	/// <param name="result"></param>trueで成功
	void SetIsPari(bool result) { isPari = result; }

	/// <summary>
	/// getter_体力
	/// </summary>
	/// <returns></returns>現在の体力
	uint32_t GetHp() { return hp; }

	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="preHp"></param>代入する体力数
	const uint32_t GetMaxHp() { return playerMaxHp; }

	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="preHp"></param>代入する体力数
	void SetHp(uint32_t preHp) { hp = preHp; }

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
	void IsShildMosion() { isShildMosion = true; }

	/// <summary>
	/// 強制的にジャンプさせる(演出等で使う)
	/// </summary>
	void IsJumping() { isJump = true; }

	/// <summary>
	/// 傘の8方向の回転
	/// </summary>
	/// <param name="direction"></param>回転角度
	void UmbrellaRange(const float& direction);

private:
	//オブジェクト
	std::unique_ptr<Object_glTF> object;

	//input
	Input* input_ = nullptr;
	XINPUT_STATE state, preState;
	//プレイヤーの速さ
	const float standard_speed = 0.14f;
	float speed = standard_speed;
	//ジャンプフラグ
	bool isJump = false;

	//重力
	const float standard_grabity = 0.01f;//重力の質量
	const float fixed_grabity = -0.05f;//滑空時重力値を固定

	/// 弾丸
	std::list<std::shared_ptr<PlayerBullet>> bullets_;
	float coolTimer = 0.0f;//クールタイム
	float coolMax = 0.5f;
	uint32_t bulletCount = 3;//一度に出る弾丸数

	//ボタン
	bool pushA = false;
	bool pushD = false;
	bool pushW = false;
	bool pushS = false;
	
	//向き
	const float upDis = 270.0f;//上
	const float downtDis = 90.0f;//下
	const float leftDis = 180.0f;//左
	const float rightDis = 360.0f;//右

	const float Naname_Value = 45.0f;//斜めにする変数

	//傘銃
	std::unique_ptr<Umbrella> umbrella = nullptr;
	WorldTransform wtGun;//傘のワールド座標系

	//傘のシールドフラグ
	bool isShield = false;
	//パリィ
	bool isPari = false;
	float pariTimeMax = 0.1f;//パリィする時間
	float pariTime = pariTimeMax;
	float pariCoolTime = 0.0f;
	const Vector3 playerFront = { 0,0,1.5f };//プレイヤーの前方

	/// ノックバック
	bool isKnockback = false;
	Vector3 backPower = { 0,0,0 };
	float KnockBackTimer = 0.0f;
	float KnockBackTimeMax = 0.5f;

	///ブリンク
	bool isBrink = false;
	bool isOneBrink = false;
	float brinkTimer = 0.0f;
	const float brinkTimeMax = 0.5f;

	//落下する時ふわふわできるように
	bool isUmbrellaFall = false;

	const uint32_t playerMaxHp = 3;//設定する体力
	//ダメージを食らった後の無敵時間
	float infinityTimer = 0.0f;
	const float infinityTimeMax = 1.0f;

	//倒された
	float deadTimer = 0.0f;
	float hitStopTime = 1.0f;
	float deadTimeMax = 3.0f;

	//復活
	bool isRespawn = false;

	//サウンド
	SoundData hitSound;//ダメージを食らった
	SoundData pariSound;//パリィに成功
	float volume = 0.07f;

	//パーティクル
	std::unique_ptr<Particle> particle_walk;//歩く
	std::unique_ptr<Particle> particle_fire;//撃つ
	std::unique_ptr<Particle> particle_brink;//ブリンク
	std::unique_ptr<Particle> particle_damage;//ダメージを食らった
	std::unique_ptr<Particle> particle_pari;//パリィ成功
	std::unique_ptr<Particle> particle_dead;//倒された演出

	//前回座標の保存場所
	Vector3 PrePosition;

	//アニメーション
	enum Animation_Mode {
		mode_stop,//止まってる
		mode_move,//動いてる
		mode_sield,//守っている
	};

	Animation_Mode animation_mode;
	Animation_Mode PreAnimation_mode;

	//UI
	std::vector<std::unique_ptr<Sprite>> sprites_Hp;
	const Vector2 textureSize_Hp = { 64,64 };//スプライトサイズ
	const Vector2 initializePoint_Hp = { 20.0f,45.0f };//スプライトの初期位置
	const float distanceY_Hp = 10.0f;//スプライトのY軸幅

	//傘がリアクションするflag
	bool isShildMosion = false;

	//アニメーションのみを動かすフラグ
	//bool performance_mode = false;
};