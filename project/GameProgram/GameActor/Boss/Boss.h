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
	/// <param name="center">初期値をセンターにに</param>
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


	void Active() override;
	void Dead() override;
	void Performance() override;

	void CommandMove()  override;
	void CommandFire(float kFrame, float bulletSpeed, uint32_t bulletMax) override;
	void CommandAroundMove() override;
	void CommandFarMove() override;
	void CommandFarTackle() override;
	void CommandFallPlayer() override;
	void CommandBeforeActionMotion() override;


	/// <summary>
	/// 移動ポイントの設定
	/// </summary>
	/// <param name="point">移動させるポイント</param>
	/// <param name="speedDivision">移動の分割数</param>
	void SetMovePoint(const Vector3& point, float speedDivision = 3.0f);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="value"></param>
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
