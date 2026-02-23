#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "Particle.h"

#include "GLTFCommon.h"
#include "Object3dCommon.h"
#include "Shadow.h"
#include "Sprite.h"

#include "UseEveryOne.h"

#include "Reaction.h"
#include "ActorState.h"

#include "CollisionManager.h"

/// <summary>
/// プレイヤー、敵のような動く、攻撃すると
/// ゲームで使う基盤クラス
/// </summary>
class GameActor : public ActorCommand, public CollisionSource
{
public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns>現在の座標位置</returns>
	Vector3 GetTranslate() const { return transform_.translate; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate">代入する座標位置</param>
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	/// <summary>
	/// getter_ワールド座標系の位置
	/// </summary>
	/// <returns>平行移動成分を搭載したプレイヤー座標</returns>
	Vector3 GetWorldPosition();

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns>現在の回転</returns>
	Vector3 GetRotate() const { return transform_.rotate; }
	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotate">代入する各回転角度</param>
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

	/// <summary>
	/// getter_スケール
	/// </summary>
	/// <returns>現在の大きさ</returns>
	Vector3 GetScale() const { return transform_.scale; }
	/// <summary>
	/// setter_スケール
	/// </summary>
	/// <param name="translate">代入する大きさ</param>
	void SetScale(const Vector3& scale) { transform_.scale = scale; }

	/// <summary>
	/// setter_当たり判定の大きさ
	/// </summary>
	/// <param name="size">大きさ</param>
	void SetColliderSize(const Vector3& size) { colliderSize_ = size; }

	/// <summary>
	/// getter_倒れた時
	/// </summary>
	/// <returns>倒れたフラグ</returns>
	bool GetIsDead() { return isDead_; }

	/// <summary>
	/// アニメーションのみを動かす関数(演出で使う)
	/// </summary>
	/// <param name="result">trueならアニメーションのみ / falseなら操作可能</param>
	void IsPerformanceFlag(bool result) { isPerformance_ = result; }

	/// <summary>
	/// 演出中であるか
	/// </summary>
	/// <returns></returns>
	bool GetPerformanceMode() { return isPerformance_; }

	/// <summary>
	/// 影のみの更新処理
	/// </summary>
	void ShadowUpdate();

	/// <summary>
	/// getter_体力
	/// </summary>
	/// <returns></returns>現在の体力
	uint32_t GetHp() { return hp_; }

	/// <summary>
	/// setter_体力
	/// </summary>
	/// <param name="preHp"></param>代入する体力数
	void SetHp(uint32_t preHp) { hp_ = preHp; }

	/// <summary>
	/// 体力の設定
	/// </summary>
	/// <param name="max">最大体力</param>
	void HP_Initialize(uint32_t max);

	/// <summary>
	/// ステートパターン変更(状態)
	/// </summary>
	/// <param name="actorState">次のステートパターン</param>
	void ChangeStatePattern(std::unique_ptr<BaseActorState> actorState);

protected:

	WorldTransform wt_;
	Transform transform_{};
	//当たり判定
	Vector3 colliderSize_;

	//体力
	uint32_t maxHp_;//最大値
	uint32_t hp_;//現在の体力

	bool isDead_ = false;//倒れたフラグ

	//向き(左右)
	const float kDirectionRight_ = 90.0f;
	const float kDirectionLeft_ = -90.0f;

	//最大角度(360度)
	const float kMaxAngle = 360.0f;

	//演出中フラグ
	bool isPerformance_ = false;

	///影
	std::unique_ptr<Shadow> shadow_;

	//ダメージのリアクション
	bool isDamageMotion_ = false;

	//ダメージリアクション
	float scaleTimer_ = 0.0f;
	//足して大きくする値
	Vector3 damageScale_ = { 0.1f, 0.1f, 0.1f };
	const float kDamageMaxTime_ = 0.3f;

	//パーティクルのコンテナ
	std::unordered_map<std::string, std::unique_ptr<Particle>> particles_;

	//リアクションクラス
	std::unique_ptr<Reaction> reaction_;

	//地面・壁で使う当たり判定
	CollisionOverlap collisionOverlap;

private:
	std::unique_ptr<BaseActorState> actorState_;
};