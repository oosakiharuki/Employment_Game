#pragma once
#include "Object_glTF.h"
#include "PlayerBullet.h"
#include "Umbrella.h"
#include "Audio.h"
#include "Input.h"

#include "GameActor.h"
#include "GravityActor.h"

#include "UIManager.h"

#include "BasePlayerState.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player : public GameActor, public GravityActor, public PlayerCommand {
public:
	Player();
	~Player();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
	/// <summary>
	/// パーティクル処理
	/// </summary>
	void DrawParticle();

	/// <summary>
	/// getter_ワールド座標系
	/// </summary>
	/// <returns></returns>プレイヤー本体のワールド座標系
	const WorldTransform& GetWorldTransform() { return wt_; }
	
	/// <summary>
	/// getter_ワールド座標系の位置
	/// </summary>
	/// <returns>平行移動成分を搭載したプレイヤー座標</returns>
	Vector3 GetWorldPosition();

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

	void SetUmbrellaRotate();

	/// <summary>
	/// getter_シールドフラグ
	/// </summary>
	/// <returns></returns>trueなら守っている
	bool GetIsShield() { return isShield_; }

	/// <summary>
	/// setter_シールドフラグ
	/// </summary>
	/// <param name="result"></param>
	void SetIsShield(bool result) { isShield_ = result; }

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
	void KnockBackPlayer(const Vector3& Power, float TimerMax);
	/// <summary>
	/// 傘が当たったノックバック
	/// </summary>
	/// <param name="Power"></param>ノックバックの強さ
	/// <param name="TimerMax"></param>ノックバックする時間(EaseInを仕様しているため)
	void KnockBackUmbrella(const Vector3& Power,float TimerMax);

	/// <summary>
	/// ノックバック共通部分
	/// </summary>
	/// <param name="TimeMax"></param>
	void KnockBackCommon(float TimeMax);

	/// <summary>
	/// 全ての敵が初期地に戻った時
	/// </summary>	
	void RespawnPlayer();

	/// <summary>
	/// getter_パリィ
	/// </summary>
	/// <returns></returns>跳ね返したフラグ
	bool GetIsParry() { return isParry_; }

	/// <summary>
	/// パリィ成功 = 連続弾も跳ね返す
	/// </summary>
	void ParrySuccess();

	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="preHp"></param>代入する体力数
	const uint32_t GetMaxHp() { return kPlayerMaxHp_; }

	/// <summary>
	/// getter_残機
	/// </summary>
	/// <returns></returns>
	const uint32_t GetRemain() { return remain_; }

	/// <summary>
	/// setter_残機
	/// </summary>
	/// <param name="remain"></param>
	void SetRemain(uint32_t remain) { remain_ = remain; }

	/// <summary>
	/// setter_影の位置
	/// </summary>
	/// <param name="position"></param>影の位置の登録
	void SetShadowPosition(const Vector3& position) { shadow_->SetTranslate(position); }

	/// <summary>
	/// 傘が当たったリアクションフラグ
	/// </summary>
	void IsShieldMotion();

	/// <summary>
	/// プレイヤーの向きををカメラに
	/// </summary>
	void DirectionTheCamera(){
		transform_.rotate.y = kPlayerFrontAngle_.y;//カメラのほうに向く;
	}

	/// <summary>
	/// プレイヤーを後ろに向かす
	/// </summary>
	void BackDirection() {
		transform_.rotate = { 0.0f,0.0f,0.0f };//初期状態が後ろを向いているため
	}

	/// <summary>
	/// ステートパターン変更(アクション)
	/// </summary>
	/// <param name="enemyState">次のステートパターン</param>
	void ChangeStatePatternAction(std::unique_ptr<BasePlayerState> state);


	/// <summary>
	///	ブリンク発動条件
	/// </summary>
	/// <returns></returns>
	bool BrinkFlag();

	/// <summary>
	/// ブリンクタイマー
	/// </summary>
	/// <returns></returns>
	bool BrinkTimeMax();

private:

	//コマンド処理
	void CommandMove() override;
	void CommandJump() override;
	void CommandFire() override;
	void CommandShield() override;
	void CommandBrink() override;

	void Active() override;
	void Dead() override;
	void Performance() override;


	/// <summary>
	/// 生存時の処理()
	/// </summary>
	void LifeUpdate();


	void ActionUpdate();


	//スプライトの変化
	void SpriteUpdate();

	/// <summary>
	/// 傘の8方向の回転
	/// </summary>
	/// <param name="direction"></param>回転角度
	void UmbrellaRange(float direction);

	/// <summary>
	/// 弾を発射する(ショットガン風)
	/// </summary>
	void ShootBullet();


	/// <summary>
	/// 滑空処理
	/// </summary>
	void Gliding();

	/// <summary>
	/// 
	/// </summary>
	void GravityDown();


	/// <summary>
	/// オブジェクトの初期化処理
	/// </summary>
	void InitMainBody();

	/// <summary>
	/// 傘の初期化処理
	/// </summary>
	void InitUmbrella();

	/// <summary>
	/// パーティクルの設定まとめ
	/// </summary>
	void InitParticles();

	/// <summary>
	/// 体力のUI
	/// </summary>
	void SettingSpriteHp(uint32_t num);

	/// <summary>
	/// 音声処理(SE,BGM)
	/// </summary>
	void InitAudio();


	/// <summary>
	/// 弾丸の更新処理
	/// </summary>
	void BulletUpdate();
	/// <summary>
	/// 無敵時間の経過処理
	/// </summary>
	void InfinityTimeUpdate();
	/// <summary>
	/// リアクションするか更新処理
	/// </summary>
	void ReactionsUpdate();
	/// <summary>
	/// アニメーション更新処理
	/// </summary>
	void AnimationUpdate();
	/// <summary>
	/// 土煙のパーティクル
	/// </summary>
	void SmockParticle();
	/// <summary>
	/// imGui更新処理
	/// </summary>
	void ImGuiUpdate();
	/// <summary>
	/// 最後のほうにする更新処理(オブジェクト更新,WorldTransform更新)
	/// </summary>
	void BehindUpdate();

	/// <summary>
	/// ノックバックする更新処理
	/// </summary>
	void KnockBackUpdate();


	//オブジェクト
	std::unique_ptr<Object_glTF> object_;

	const float kFixedGravityPower_ = -0.05f;//滑空時重力値を固定

	/// 弾丸
	std::list<std::shared_ptr<PlayerBullet>> bullets_;
	float fireCoolTimer_ = 0.0f;//クールタイマー
	const float kFireCoolTimeMax_ = 0.5f;//クールタイム最大時間
	const uint32_t kBulletCount_ = 3;//一度に出る弾丸数
	
	const float kDispersionBetween_ = 0.1f;//分散する間
	const float kBulletSpeed_ = 0.5f;//弾丸の前方向の速さ


	//向き
	const float kUpDis_ = 270.0f;//上
	const float kDownDis_ = 90.0f;//下
	const float kLeftDis_ = 180.0f;//左
	const float kRightDis_ = 360.0f;//右

	const float kDiagonalValue_ = 45.0f;//斜めにする変数
	const float kPlayerFrontRange_ = 180.0f;//プレイヤーがカメラから見て正面を向く

	//ゴール時前を向くように
	const Vector3 kPlayerFrontAngle_ = { 0.0f,180.0f,0.0f };

	//円柱または円錐のパーティクルを横向きにするための角度
	const float kNinetyAngle_ = 90.0f;
	//パーティクル用傘の方向
	Vector3 umbrellaRange_ = { 0.0f,0.0f,0.0f };

	//傘銃
	std::unique_ptr<Umbrella> umbrella_ = nullptr;
	WorldTransform wtGun_;//傘のワールド座標系
	Transform transformGun_{};


	/// ノックバック
	bool isKnockback_ = false;
	Vector3 backPower_ = { 0,0,0 };
	float knockBackTimer_ = 0.0f;
	float knockBackTimeMax_ = 0.0f;//最大ノックバック時間
	const Vector3 kBulletKnockbackPower_ = { 0.0f,0.0f,0.1f };//撃った場合のノックバックパワー

	const uint32_t kPlayerMaxHp_ = 3;//設定する体力
	//残機(remain)
	uint32_t remain_;

	//ダメージを食らった後の無敵時間
	float infinityTimer_ = 0.0f;
	const float kInfinityTimeMax_ = 1.0f;//最大無敵時間

	//倒された
	float deadTimer_ = 0.0f;
	const float kHitStopTime_ = 1.0f;//ヒットストップ
	const float kDeadTimeMax_ = 3.0f;//死んだ演出用時間
	const float kPlayerDeadRotating_ = 10.0f;//10度ずつ回る
	const float kDeadLittleUp_ = 0.4f;

	//サウンド
	SoundData hitSound_;//ダメージを食らった
	SoundData parrySound_;//パリィに成功
	const float kVolume_ = 0.07f;//ボリューム

	//-パーティクル-
	//歩く
	ParticleParameters particleWalk_ = {
		"player_walk", "resource/Sprite/white.dds", Primitive::CreateBox(), 5, 0.15f, {1,1,1}
	};
	//ブリンク
	ParticleParameters particleBrink_ = {
		"player_brink","resource/Sprite/cone.dds", Primitive::CreateCone(), 1, 0.3f, {2.0f,2.0f,2.0f}
	}; 
	//撃つ
	ParticleParameters  particleFire_ = {
		"player_fire","resource/Sprite/cone.dds", Primitive::CreateCone(), 1, 0.3f,{1,1,1}
	};
	//ダメージを食らった
	ParticleParameters particleDamage_ = {
		"player_damage", "resource/Sprite/circle.dds", Primitive::CreateRing(), 20, 0.6f,{1,1,1}
	};
	//パリィ成功
	ParticleParameters particleParry_ = {
		"player_parry", "resource/Sprite/white.dds", Primitive::CreateCone(), 1, 0.5f,{2.0f,0.2f,2.0f}
	};
	//倒された演出
	ParticleParameters particleDead_ = {
		"player_dead", "resource/Sprite/white.dds", Primitive::CreateSphere(), 3,0.1f,{0.5f, 0.5f, 0.5f}
	};

	//前回座標の保存場所
	Vector3 prePosition_;
	
	/// <summary>
	/// プレイヤーが動いているか判定
	/// </summary>
	/// <returns></returns>
	const bool IsMovePosition();

	//オブジェクトたち
	std::unordered_map<std::string, std::string> objectMotions_;

	//オブジェクト / アニメーション
	std::string motionName_ = "standby";    //現在のアニメーション
	std::string preMotionName_ = "standby"; //前回のアニメーション

	//UI
	const Vector2 kTextureSizeHp_ = { 64,64 };//スプライトサイズ
	const Vector2 kInitializePointHp_ = { 20.0f,45.0f };//スプライトの初期位置
	const float kDistanceYHp_ = 10.0f;//スプライトのY軸幅

	//傘がリアクションするflag
	bool isShieldMotion_ = false;

	//ステートパターン
	//プレイヤーの操作アクション用
	std::unique_ptr<BasePlayerState> actionState_;
};