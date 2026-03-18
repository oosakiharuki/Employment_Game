#pragma once
#include "Object_glTF.h"
#include "Umbrella.h"
#include "Audio.h"
#include "Input.h"

#include "GravityActor.h"

#include "UIManager.h"

#include "BasePlayerState.h"

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
	const WorldTransform& GetWorldTransform() { return wt_; }
	
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
	/// パリィ成功 = 連続弾も跳ね返す
	/// </summary>
	void ParrySuccess();

	/// <summary>
	/// パーティクル攻撃発動時
	/// </summary>
	/// <param name="translate"></param>
	void ParticleFire(const Vector3& translate);

	/// <summary>
	/// パーティクルブリンク発動時
	/// </summary>
	void ParticleBrink();

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
	/// ステートパターン変更(アクション)
	/// </summary>
	/// <param name="enemyState">次のステートパターン</param>
	void ChangeStatePatternAction(std::unique_ptr<BasePlayerState> state);

	/// <summary>
	/// リスポーンフラグ
	/// </summary>
	/// <returns>リスポーンフラグ</returns>
	bool IsRespawn() { return isRespawn_; }

	/// <summary>
	/// リスポーン終了
	/// </summary>
	void RespawnEnd() { isRespawn_ = false; }

private:
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
	/// 生存時の処理()
	/// </summary>
	void LifeUpdate();

	/// <summary>
	/// アクションコマンド更新処理(ジャンプ、発砲攻撃など)
	/// </summary>
	void ActionUpdate();

	/// <summary>
	/// スプライトの変化
	/// </summary>
	void SpriteUpdate();

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
	/// プレイヤーが動いているか判定
	/// </summary>
	/// <returns></returns>
	const bool IsMovePosition();

	//オブジェクト
	std::unique_ptr<Object_glTF> object_;

	const float kFixedGravityPower_ = -0.05f;//滑空時重力値を固定

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


	//落ちる限界
	const float kFallEndY_ = -10.0f;

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

	//オブジェクトたち
	std::unordered_map<std::string, std::string> objectMotions_;

	//オブジェクト / アニメーション
	std::string motionName_ = "standby";    //現在のアニメーション
	std::string preMotionName_ = "standby"; //前回のアニメーション

	//UI
	const Vector2 kTextureSizeHp_ = { 64,64 };//スプライトサイズ
	const Vector2 kInitializePointHp_ = { 20.0f,45.0f };//スプライトの初期位置
	const float kDistanceYHp_ = 10.0f;//スプライトのY軸幅

	bool isEvent_ = false;

	Vector3 kMoveMax_ = { 1000.0f,1000.0f,1000.0f };//最大移動
	Vector3 eventMin{};
	Vector3 eventMax{};

	//ステートパターン
	//プレイヤーの操作アクション用
	std::unique_ptr<BasePlayerState> actionState_;
	//リスポーンフラグ
	bool isRespawn_ = false;

	float appearanceAnimationTimer_ = 0.0f;
	const float appearanceAnimationFinishTime_ = 2.5f;

	//スタート演出(水たまりから飛び出る感じに)
	Vector3 playerPoint_{};

	const float kStartPointY_ = -10.0f;
	float pointY_ = kStartPointY_;
	//スタート時にプレイヤーを上げる変数
	const float kPlayerUp_ = 0.1f;

	std::unique_ptr<PlayerCommand> actionCommand_;

	const Vector3 kPlayerFront_ = { 0,0,1.5f };//プレイヤーの前方

};