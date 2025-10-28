#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "Object_glTF.h"
#include "PlayerBullet.h"
#include "Umbrella.h"
#include "Audio.h"
#include "Input.h"
#include "Particle.h"

#include "GLTFCommon.h"
#include "Object3dCommon.h"

#include "Shadow.h"
#include "Sprite.h"

/// <summary>
/// プレイヤー
/// </summary>
class Player {
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
	/// getter_座標
	/// </summary>
	/// <returns></returns>現在の座標
	Vector3 GetTranslate() { return worldTransform.translation_; }
	/// <summary>
	/// setter_座標
	/// </summary>
	/// <param name="translate"></param>代入する座標
	void SetTranslate(Vector3 translate) { worldTransform.translation_ = translate; }

	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotate"></param>代入する各回転角度
	void SetRotate(Vector3 rotate) { worldTransform.rotation_ = rotate; }
	
	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns></returns>座標が中心のAABB
	AABB GetAABB();
	/// <summary>
	/// setter_当たり判定AABB
	/// </summary>
	/// <param name="aabb"></param>AABB
	void SetAABB(AABB aabb) { playerAABB = aabb; }

	/// <summary>
	/// 地面判定フラグ変更
	/// </summary>
	/// <param name="result"></param>trueは地面 / falseは空中
	void IsGround(bool result);

	/// <summary>
	/// getter_地面判定
	/// </summary>
	/// <returns></returns>現在の地面判定
	bool GetIsGround() { return isGround; }

	/// <summary>
	/// getter_ワールド座標系
	/// </summary>
	/// <returns></returns>プレイヤー本体のワールド座標系
	const WorldTransform& GetWorldTransform() { return worldTransform; }

	/// <summary>
	/// 弾を発射する(ショットガン風)
	/// </summary>
	void ShootBullet();

	//Getterはshared_ptrのほうが適任かも

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
	void IsDamage();
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
	/// getter_ワールド座標系の位置
	/// </summary>
	/// <returns></returns>平行移動成分を搭載したプレイヤー座標
	Vector3 GetWorldPosition();

	/// <summary>
	/// setter_リスポーンポイント
	/// </summary>
	/// <param name="translate"></param>各ステージの最初の位置/チェックポイント
	void SetRespownPosition(Vector3 translate) { respownPosition = translate; }
	
	/// <summary>
	/// 死んだときの処理
	/// </summary>
	void DeadPlayer();
	/// <summary>
	/// プレイヤーが倒れた時
	/// </summary>
	/// <returns></returns>倒れたフラグ
	bool GetIsPlayerDown() { return isPlayerDown; }
	/// <summary>
	/// getter_復活
	/// </summary>
	/// <returns></returns>復活フラグ
	bool GetIsRespown() { return isRespown; }
	
	/// <summary>
	/// 全ての敵が初期地に戻った時
	/// </summary>	
	void AllRespownEnd();

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
	uint32_t GetHp() { return Hp; }

	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="preHp"></param>代入する体力数
	void SetHp(uint32_t preHp) { Hp = preHp; }

	/// <summary>
	/// setter_影の位置
	/// </summary>
	/// <param name="position"></param>影の位置の登録
	void SetShadowPosition(Vector3 position) { shadow_->SetTranslate(position); }

	/// <summary>
	/// getter_影の当たり判定
	/// </summary>
	/// <returns></returns>影のAABB
	AABB GetShadowAABB() { return shadow_->GetAABB(); }

	/// <summary>
	/// 影のみの更新処理
	/// </summary>
	void ShadowUpdate();

	//スプライトの変化
	void SpriteUpdate();

	/// <summary>
	/// リアクション(拡大縮小)
	/// </summary>
	/// <param name="mosionOn"></param>リアクションフラグ
	/// <param name="scale"></param>どのくらい大きくするか
	/// <param name="maxTime"></param>リアクションタイマーの最大値
	void ScaleUpdate(bool* mosionOn, Vector3 scale, const float maxTime);

	/// <summary>
	/// 傘が当たったリアクションフラグ
	/// </summary>
	void IsShildMosion() { isShildMosion = true; }

	/// <summary>
	/// アニメーションのみを動かす関数(演出で使う)
	/// </summary>
	/// <param name="anser"></param>
	/// trueならアニメーションのみ / falseなら操作可能
	void IsAnimationOnlyUpdate(const bool& anser) { isAnimationOnlyUpdate = anser; }

	/// <summary>
	/// 強制的にジャンプさせる(演出等で使う)
	/// </summary>
	void IsJumping() { isJump = true; }

private:
	//オブジェクト
	std::unique_ptr<Object_glTF> object;
	WorldTransform worldTransform;
	//当たり判定
	AABB playerAABB;

	//input
	Input* input_ = nullptr;
	XINPUT_STATE state, preState;
	//プレイヤーの速さ
	const float standard_speed = 0.14f;
	float speed = standard_speed;
	//ジャンプフラグ
	bool isJump = false;

	// 地面判定フラグ{stageの当たり判定で使う}
	bool isGround = false;

	//重力
	float grabity = 0.0f;
	//秒数時間
	const float deltaTime = 1.0f / 60.0f;

	/// 弾丸
	std::list<std::shared_ptr<PlayerBullet>> bullets_;
	float coolTimer = 0.0f;//クールタイム
	float coolMax = 0.5f;
	uint32_t bulletCount = 3;//一度に出る弾丸数
	
	//プレイヤーの向き
	enum Direction {
		right,//右
		left  //左
	};

	///射程範囲 8方向(順番は上から時計回り)
	enum Range {
		Up,
		UpRight,
		Right,
		DownRight,
		Down,
		DownLeft,
		Left,
		UpLeft,
	};

	Direction direction = Direction::right;
	Range range = Range::Right;

	//角度が変わった時のフラグ
	bool isChangeDirection = false;

	//傘銃
	std::unique_ptr<Umbrella> umbrella = nullptr;
	WorldTransform wtGun;//傘のワールド座標系
	float umbrellaNormal = 0.0f;

	//傘のシールドフラグ
	bool isShield = false;
	//パリィ
	bool isPari = false;
	float pariTimeMax = 0.1f;//パリィする時間
	float pariTime = pariTimeMax;
	float pariCoolTime = 0.0f;

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

	//体力
	uint32_t MaxHp = 3;
	uint32_t Hp = MaxHp;
	//ダメージを食らった後の無敵時間
	float infinityTimer = 0.0f;
	const float infinityTimeMax = 1.0f;

	//倒された
	float deadTimer = 0.0f;
	float hitStopTime = 1.0f;
	float deadTimeMax = 3.0f;
	bool isPlayerDown = false;

	//復活
	Vector3 respownPosition;
	bool isRespown = false;

	//サウンド
	SoundData hitSound;//ダメージを食らった
	SoundData pariSound;//パリィに成功

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


	///影
	std::unique_ptr<Shadow> shadow_;

	//UI
	std::vector <std::unique_ptr<Sprite>> sprites_Hp;

	//リアクション
	bool isDamageMosion = false;
	bool isShildMosion = false;

	//ダメージのリアクション拡大縮小パラメーター
	Vector3 damageScale = { 0.1f, 0.1f, 0.1f };
	const float damageMaxTime = 0.14f;
	float scaleTimer = 0.0f;

	Vector3 defaultScale = { 1,1,1 };//元の大きさ

	//アニメーションのみを動かすフラグ
	bool isAnimationOnlyUpdate = false;
};