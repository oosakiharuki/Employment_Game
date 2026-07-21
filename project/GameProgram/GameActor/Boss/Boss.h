#pragma once
#include "Object_glTF.h"
#include "WorldTransform.h"
#include "MyMath.h"
#include "EnemyBullet.h"

#include "BaseBossState.h"
#include "Player.h"

#include "GameActor.h"

#include "CollisionManager.h"

/// <summary>
/// ボス(クラッコっぽい動き)
/// </summary>
class Boss : public GameActor, public BossCommand
{
public:
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
	/// 移動用に真ん中を設定
	/// </summary>
	/// <param name="center">初期値をセンターに</param>
	void BossCenter(const Vector3& center);

	/// <summary>
	/// setter_プレイヤー
	/// </summary>
	/// <param name="player">ゲームで操作しているプレイヤー</param>
	void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// getter_行動カウント
	/// </summary>
	/// <returns>カウント数</returns>
	uint32_t ActionCount() { return actionCount_; }

	/// <summary>
	/// ダメージ
	/// </summary>
	void IsDamage();

	/// <summary>
	/// 死亡モーションが終わった時
	/// </summary>
	/// <returns>trueでモーションが完了した</returns>
	bool IsDeadMotionFinish() { return isDeadMotionFinish_; }

	/// <summary>
	/// 行動が終了した時
	/// </summary>
	/// <returns>trueなら終了</returns>
	bool GetMotionFinish() { return motionFinish_; }

	/// <summary>
	/// ステートパターン変更処理
	/// </summary>
	/// <param name="playerState">次のステートパターン</param>
	void ChangeStatePattern(std::unique_ptr<BaseBossState> state);

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
	/// 移動コマンド
	/// </summary>
	void CommandMove()  override;
	/// <summary>
	/// 弾丸を発砲する攻撃コマンド
	/// </summary>
	/// <param name="kFrame">弾の間 〇秒数</param>
	/// <param name="bulletSpeed">弾丸速度</param>
	/// <param name="bulletMax">弾丸数</param>
	void CommandFire(float kFrame, float bulletSpeed, uint32_t bulletMax) override;
	/// <summary>
	/// 下から回る移動コマンド
	/// </summary>
	void CommandAroundMove() override;
	/// <summary>
	/// ステージの奥に移動コマンド
	/// </summary>
	void CommandFarMove() override;
	/// <summary>
	/// 奥側から発砲攻撃コマンド
	/// </summary>
	void CommandFarTackle() override;
	/// <summary>
	/// プレイヤーに向かって突進攻撃コマンド
	/// </summary>
	void CommandFallPlayer() override;
	/// <summary>
	/// プレイヤーの真上から落ちてくる攻撃コマンド
	/// </summary>
	void CommandBeforeActionMotion() override;

	/// <summary>
	/// 移動ポイントの設定
	/// </summary>
	/// <param name="point">移動させるポイント</param>
	/// <param name="speedDivision">移動時間(三秒をデフォルトとする)</param>
	void SetMovePoint(const Vector3& point, float speedDivision = 3.0f);

	/// <summary>
	/// setter_移動セグメントのオリジン
	/// </summary>
	/// <param name="value">座標を設定</param>
	void SetOrigin(const Vector3& value) { move_.origin = value; }

	/// <summary>
	/// 弾丸攻撃
	/// </summary>
	void FireBullet();

	/// <summary>
	/// イージング移動
	/// </summary>
	void EaseMove();
	/// <summary>
	/// イージング移動が完了したフラグ
	/// </summary>
	/// <returns>trueで成功(タイマーもリセット)</returns>
	bool EaseGoal();
	/// <summary>
	/// imGui更新処理
	/// </summary>
	void ImGuiUpdate();

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
	/// 体力バースプライト更新	 
	/// </summary>
	void HpSpriteUpdate();

	std::unique_ptr<EngineLayer::Object_glTF> object_;
	//弾丸
	std::list<std::unique_ptr<EnemyBullet>> bullets_;
	//ボスの最大体力
	const uint32_t kMaxHp_ = 90;

	//プレイヤークラス(追尾用)
	Player* player_ = nullptr;

	//リアクション
	bool isDamageReaction_ = false;
	Vector3 damageReactionPower_ = { 0.05f,0.05f ,0.05f };
	float damageReactionTimer_ = 0.0f;
	const float kDamageReactionTimeMax_ = 0.3f;

	//死んだときの処理
	const float kShakePower = 0.25f;
	Vector3 deadPosition_{};
	Vector3 deadScale_{};
	float deadTimer_ = 0.0f;
	const float kDeadTimeMax_ = 6.0f;
	//死亡モーションも終了したフラグ
	bool isDeadMotionFinish_ = false;

	//ステートパターン
	std::unique_ptr<BaseBossState> bossState_;

	//真ん中座標、左右移動に使う
	Vector3 moveCenter_{};

	Vector3 targetPosition_;

	//低空移動のパラメータ
	const float kAroundRightX_ = 20.0f;//右端
	const float kAroundLeftX_ = -20.0f;//左端
	
	//どのくらい下がるか
	const float kAroundUnder_ = 1.0f;
	//標準スピード
	const float kStandardMoveSpeed_ = 3.0f;

	EngineLayer::SoundData fireSound_;// "resource/Sound/fire.mp3"
	EngineLayer::SoundData fireBeforeActionSound_;//"resource/Sound/enemyBeforeFire.mp3"

	const float kVolume_ = 0.25f;


	//コマンドの行動を終えたフラグ(次のコマンドへ移動の合図となる)
	bool motionFinish_ = false; 


	//体力スプライト
	std::unique_ptr<EngineLayer::Sprite> hpSprite_;
	std::unique_ptr<EngineLayer::Sprite> underBarSprite_;

	//パラメータ
	const Vector2 kSpriteWindowsPosition_ = { 0.7f,0.85f };

	const Vector2 kHpSpriteSize_ = { 256,64 };
	const float kSpriteRatio_ = 0.1f;

	//影の長さ、大きさ
	Vector3 shadowLength = { 1.0f, 18.0f ,1.0f };
	const float kShadowDirection = 10.0f;
};
