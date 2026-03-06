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
	void SetBossCenter(const Vector3& center) { moveCenter_ = center; }

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
	/// <param name="speedDivision">移動の分割数</param>
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
	/// 線形保管の移動
	/// </summary>
	void LerpMove();
	/// <summary>
	/// 線形保管の移動が完了したフラグ
	/// </summary>
	/// <returns>trueで成功(タイマーもリセット)</returns>
	bool LerpGoal();

	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;

	std::unique_ptr<Object_glTF> object_;

	std::list<std::unique_ptr<EnemyBullet>> bullets_;

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

	bool isDeadMotionFinish_ = false;

	/// <summary>
	/// imGui更新処理
	/// </summary>
	void ImGuiUpdate();
	
	//ステートパターン
	std::unique_ptr<BaseBossState> bossState_;

	//真ん中座標、左右移動に使う
	Vector3 moveCenter_{};

	//new
	bool motionFinish_ = false; 
};
