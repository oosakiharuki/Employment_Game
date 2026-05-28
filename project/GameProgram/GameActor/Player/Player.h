#pragma once
#include "Object_glTF.h"
#include "BaseUmbrella.h"
#include "Umbrella.h"
#include "Audio.h"
#include "Input.h"

#include "GravityActor.h"

#include "UIManager.h"

#include "PlayerActionsInputHandler.h"

#include "ReinforceGauge.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player : public GravityActor {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();
	/// <summary>
	/// デストラクタ
	/// </summary>
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
	const EngineLayer::WorldTransform& GetWorldTransform() { return wt_; }
	
	/// <summary>
	/// getter_ワールド座標系の位置
	/// </summary>
	/// <returns>平行移動成分を搭載したプレイヤー座標</returns>
	Vector3 GetWorldPosition();
	/// <summary>
	/// getter_シールドモード
	/// </summary>
	/// <returns>傘がシールドモードであるか</returns>
	bool GetUmbrellaShieldMode() { return umbrella_->GetShieldMode(); }
	/// <summary>
	/// getter_傘の行列
	/// </summary>
	/// <returns>現在の傘の行列</returns>
	const Matrix4x4& GetUmbrellaMatWorld() { return wtGun_.GetMatWorld(); }
	/// <summary>
	/// getter_傘座標
	/// </summary>
	/// <returns>現在の傘の座標</returns>
	const Vector3& GetUmbrellaTranslate() { return umbrella_->GetTranslate(); }
	/// <summary>
	/// getter_傘回転
	/// </summary>
	/// <returns>現在の傘の回転角度</returns>
	const Vector3& GetUmbrellaRotate() { return umbrella_->GetRotate(); }
	/// <summary>
	/// setter_傘回転
	/// </summary>
	/// <param name="rotate">傘を回転する角度</param>
	void SetUmbrellaRotate(const Vector3& rotate) { transformGun_.rotate = rotate; }

	/// <summary>
	/// 傘の初期回転角度
	/// </summary>
	void InitUmbrellaRotateY() {
		//プレイヤーと同じY方向
		transformGun_.rotate.y = transform_.rotate.y;
	}

	/// <summary>
	/// シールド発動
	/// </summary>
	void OnUmbrellaShield();
	/// <summary>
	/// シールド解除
	/// </summary>
	void OffUmbrellaShield();

	/// <summary>
	/// 発砲
	/// </summary>
	void FireBulletUmbrella();

	/// <summary>
	/// ダメージを食らった
	/// </summary>
	/// <param name="hitPoint">当たった場所</param>
	void IsDamage(const Vector3& hitPoint);
	/// <summary>
	/// 落下した(即死)
	/// </summary>
	void IsFall();

	/// <summary>
	/// プレイヤーが当たったノックバック
	/// </summary>
	/// <param name="Power">ノックバックの強さ</param>
	/// <param name="TimerMax">ノックバックする時間(EaseInを仕様しているため)</param>
	void KnockBackPlayer(const Vector3& Power, float TimerMax);
	/// <summary>
	/// 傘が当たったノックバック
	/// </summary>
	/// <param name="Power">ノックバックの強さ</param>
	/// <param name="TimerMax">ノックバックする時間(EaseInを仕様しているため)</param>
	void KnockBackUmbrella(const Vector3& Power,float TimerMax);

	/// <summary>
	/// ノックバック共通部分
	/// </summary>
	/// <param name="TimeMax">ノックバックする時間(EaseInを仕様しているため)</param>
	void KnockBackCommon(float TimeMax);

	/// <summary>
	/// 全ての敵が初期地に戻った時
	/// </summary>	
	void RespawnPlayer();

	/// <summary>
	/// パーティクル攻撃発動時
	/// </summary>
	/// <param name="translate"></param>
	void ParticleFire(const Vector3& translate);


	/// <summary>
	/// パーティクルジャンプ
	/// </summary>
	void ParticleJump();

	/// <summary>
	/// setter_ジャンプ強度
	/// </summary>
	/// <param name="power"></param>
	void SetJumpPower(float power) { jumpPower_ = power; }

	/// <summary>
	/// パーティクルブリンク発動時
	/// </summary>
	void ParticleBrink();

	/// <summary>
	/// setter_ブリンク時間
	/// </summary>
	/// <param name="brinkTime">時間を設定する</param>
	void SetBrinkTimer(float brinkTime) { brinkTimer_ = brinkTime; }
	/// <summary>
	/// getter_ブリンク時間
	/// </summary>
	/// <returns>現在のブリンク時間</returns>
	float GetBrinkTimer() { return brinkTimer_; }
	/// <summary>
	/// ブリンクの発動条件
	/// </summary>
	/// <returns>ブリンクをまだしていない+1ゲージたまっている状態なら true</returns>
	bool BrinkFlag();
	/// <summary>
	/// ブリンクの時間
	/// </summary>
	/// <returns></returns>
	bool BrinkTimeMax();
	/// <summary>
	/// ブリンクが発動したフラグ
	/// </summary>
	void IsOneBrink() { isOneBrink_ = true; }

	/// <summary>
	/// パーティクルブリンクを止める
	/// </summary>
	void StopParticleBrink();

	/// <summary>
	/// プレイヤーが動いているか判定
	/// </summary>
	/// <returns></returns>
	const bool IsMovePosition();

	/// <summary>
	/// 重力をゆっくりにする(滑空)
	/// </summary>
	void GravityDown();

	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="preHp">代入する体力数</param>
	const uint32_t GetMaxHp() { return kPlayerMaxHp_; }

	/// <summary>
	/// getter_残機
	/// </summary>
	/// <returns>残機</returns>
	const uint32_t GetRemain() { return remain_; }

	/// <summary>
	/// setter_残機
	/// </summary>
	/// <param name="remain">残機の設定</param>
	void SetRemain(uint32_t remain) { remain_ = remain; }

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
	/// リスポーンフラグ
	/// </summary>
	/// <returns>リスポーンフラグ</returns>
	bool IsRespawn() { return isRespawn_; }

	/// <summary>
	/// リスポーン終了
	/// </summary>
	void RespawnEnd() { isRespawn_ = false; }

	/// <summary>
	/// ゲージポイント加算
	/// </summary>
	void AddGaugePoint() { reinforceGauge_->AddPoint(); }
	/// <summary>
	/// 強化行動の使用ポイントがあるか
	/// </summary>
	/// <returns>3ポイント以上で使用可能(true)</returns>
	bool UseGaugePoint();
	/// <summary>
	/// 使ったポイント分をマイナスする
	/// </summary>
	void SubGaugePoint() { reinforceGauge_->UsePoint(); }

	/// <summary>
	/// 傘の変更
	/// </summary>
	/// <param name="nextUmbrella">変更したい傘クラス</param>
	void WeaponChangeUmbrella(std::unique_ptr<BaseUmbrella> nextUmbrella, uint32_t num);

	/// <summary>
	/// 弾丸を追加
	/// </summary>
	/// <param name="bullet">弾丸クラス</param>
	void AddBullet(std::unique_ptr<PlayerBullet> bullet);

	/// <summary>
	/// 発砲しているフラグ
	/// </summary>
	/// <returns>しているならtrue</returns>
	bool IsFiring() { return isFiring_; }

	/// <summary>
	/// 発砲を終了させる
	/// </summary>
	void FireFinish() { isFiring_ = false; }

	/// <summary>
	/// getter_傘の番号
	/// </summary>
	/// <returns>現在の傘(武器)の対応の番号</returns>
	int GetWeaponNum() { return weaponNum_; }

	/// <summary>
	/// 無敵時間の発動
	/// </summary>
	void InfinityTime() { infinityTimer_ = 0.0f; }

private:
	/// ---状態遷移---
	/// <summary>
	/// 生存状態
	/// </summary>
	void Active() override;
	/// <summary>
	/// 死亡状態
	/// </summary>
	void Dead() override;
	/// <summary>
	/// 演出状態
	/// </summary>
	void Performance() override;

	/// <summary>
	/// 生存時の処理()
	/// </summary>
	void LifeUpdate();

	/// <summary>
	/// アクションコマンド更新処理(ジャンプ、発砲攻撃など)
	/// </summary>
	void ActionUpdate();

	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;
	/// <summary>
	/// 当たり判定をとるタイプかをチェック
	/// </summary>
	/// <param name="collisionType">相手の当たり判定タイプ</param>
	/// <returns>該当するタイプがあるなら true</returns>
	bool TypeCheckUp(const CollisionTypes& collisionType) override;

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
	/// スプライトの変化
	/// </summary>
	void SpriteUpdate();

	/// <summary>
	/// 体力のUI
	/// </summary>
	void SettingSpriteHp(uint32_t num);

	/// <summary>
	/// 音声処理(SE,BGM)
	/// </summary>
	void InitAudio();

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

	/// <summary>
	/// 敵の攻撃に当たった時(回避かダメージか)
	/// </summary>
	/// <param name="collision">当たっているコリジョンリソースクラス</param>
	void EnemyCollision(CollisionSource* collision);

	// --- オブジェクト ---
	
	//プレイヤー本体
	std::unique_ptr<EngineLayer::Object_glTF> object_;
	//傘の銃
	std::unique_ptr<BaseUmbrella> umbrella_ = nullptr;
	EngineLayer::WorldTransform wtGun_;//傘のワールド座標系
	Transform transformGun_{};

	/// 弾丸
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	// --------------------

	// --- 方向系統 ---
	
	//ゴール時前を向くように
	const Vector3 kPlayerFrontAngle_ = { 0.0f,180.0f,0.0f };

	//円柱または円錐のパーティクルを横向きにするための角度
	const float kNinetyAngle_ = 90.0f;
	//パーティクル用傘の方向
	Vector3 umbrellaRange_ = { 0.0f,0.0f,0.0f };

	const Vector3 kPlayerFront_ = { 0,0,1.5f };//プレイヤーの前方
	
	// -----------------


	// --- ノックバック ---

	bool isKnockback_ = false;
	Vector3 backPower_ = { 0,0,0 };
	float knockBackTimer_ = 0.0f;
	float knockBackTimeMax_ = 0.0f;//最大ノックバック時間

	// --------------------

	// --- ダメージ、生存関係 ---

	const uint32_t kPlayerMaxHp_ = 4;//設定する体力
	//残機(remain)
	uint32_t remain_;
	//落ちる限界
	const float kFallEndY_ = -10.0f;

	//ダメージを食らった後の無敵時間
	const float kInfinityTimeMax_ = 2.0f;//最大無敵時間
	float infinityTimer_ = kInfinityTimeMax_;

	//リスポーンフラグ
	bool isRespawn_ = false;

	//無敵中の点滅
	bool isChangeColor_ = false;//色変更
	const float kBlinkingTime_ = 0.25f;//刻む秒数

	// ---------------------------

	//　--- やられ演出 ---
	
	float deadTimer_ = 0.0f;
	const float kHitStopTime_ = 1.0f;//ヒットストップ
	const float kDeadTimeMax_ = 3.0f;//死んだ演出用時間
	const float kPlayerDeadRotating_ = 10.0f;//10度ずつ回る
	const float kDeadLittleUp_ = 0.4f;//少しだけ宙に浮かす
	 
	//--------------------
	  
	// --- サウンド ---

	const std::string kHitSoundName_ = "resource/Sound/damage.mp3";//ダメージを食らった
	const std::string kJumpSoundName_ = "resource/Sound/jump.mp3";//ジャンプ
	const float kVolume_ = 0.3f;//ボリューム

	// -----------------

	// --- パーティクル ---
	//歩く
	const std::string& particleWalk_ = "player_walk";
	const Vector3 kParticleWalkPoint_ = { 0.0f,-1.0f,-0.3f };
	Vector3 prePosition_;//前回座標の保存場所
	//ジャンプ
	const std::string& particleJump_ = "player_jump";
	//ブリンク
	const std::string& particleBrink_ = "player_brink";
	const Vector3 kLookToCameraDirection_ = { 0.0f,15.0f,0.0f };//カメラから見やすくする
	//撃つ
	const std::string& particleFire_ = "player_fire";
	//ダメージを食らった
	const std::string& particleDamage_ = "player_damage";
	//パリィ成功
	const std::string& particleParry_ = "player_parry";
	//倒された演出
	const std::string& particleDead_ = "player_dead";

	// ---------------------

	// --- アニメーション ---

	std::unordered_map<std::string, std::string> objectMotions_;
	std::string motionName_ = "standby";    //現在のアニメーション
	std::string preMotionName_ = "standby"; //前回のアニメーション

	float appearanceAnimationTimer_ = 0.0f;
	const float appearanceAnimationFinishTime_ = 2.5f;

	// ----------------------
	
	// --- UI ---
	
	const Vector2 kTextureSizeHp_ = { 64,64 };//スプライトサイズ
	const Vector2 kInitializePointHp_ = { 20.0f,45.0f };//スプライトの初期位置
	const float kDistanceYHp_ = 10.0f;//スプライトのY軸幅
	std::vector<std::unique_ptr<EngineLayer::Sprite>> hpSprites_;

	// -----------

	// ---イベントが発動した時---
	
	bool isEvent_ = false;
	Vector3 kMoveMax_ = { 1000.0f,1000.0f,1000.0f };//最大移動
	Vector3 eventMin{};
	Vector3 eventMax{};

	// --------------------------

	// --- 操作コマンド ---

	//コマンドを出力するクラス
	std::unique_ptr<PlayerActionsInputHandler> playerActionsInputHandler_;
	//コマンド
	std::vector<std::unique_ptr<BaseCommand>> command_;
	//強化ゲージ
	std::unique_ptr<ReinforceGauge> reinforceGauge_ = nullptr;

	//--移動--
	//プレイヤーの速さ
	const float kStandardSpeed_ = 0.14f;//通常の速さ
	float speed_ = kStandardSpeed_;

	//--ジャンプ--
	float jumpPower_ = 0.0f;//上がる高さ
	float prevDirectionWidth_ = 0.0f;//横の向き
	const float kFixedGravityPower_ = -0.05f;//滑空時重力値を固定

	//--ブリンク--
	bool isOneBrink_ = false;//一回のみ
	//ブリンク時間
	float brinkTimer_ = 0.0f;
	const float kBrinkTimeMax_ = 0.5f;//最大値
	//傘の位置設定時に使う
	const float kBrinkPower_ = 1.25f;

	//スローがかかる時間
	const float kSlowTime_ = 2.0f;

	//
	int weaponNum_ = 0;

	bool isFiring_ = false;

	float fireCoolTimer_ = 0.0f;//クールタイマー
	const float kFireCoolTimeMax_ = 0.5f;//クールタイム最大時間

	// -------------------------
};