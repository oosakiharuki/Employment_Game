#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "Particle.h"

#include "GLTFCommon.h"
#include "Object3dCommon.h"
#include "Shadow.h"
#include "Sprite.h"

/// <summary>
/// プレイヤー、敵の関数変数を使う基盤クラス
/// </summary>
class GameActor
{
public:

	/// <summary>
	/// アクター内でもできる初期化処理
	/// </summary>
	void Actor_InitializeCommon();

	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns></returns>現在の座標位置
	Vector3 GetTranslate() { return wt.translation_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>代入する座標位置
	void SetTranslate(Vector3 translate) { wt.translation_ = translate; }

	/// <summary>
	/// getter_ワールド座標系の位置
	/// </summary>
	/// <returns></returns>平行移動成分を搭載したプレイヤー座標
	Vector3 GetWorldPosition();

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns></returns>現在の回転
	Vector3 GetRotate() { return wt.rotation_; }
	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotate"></param>代入する各回転角度
	void SetRotate(Vector3 rotate) { wt.rotation_ = rotate; }
	
	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns></returns>現在の座標位置
	Vector3 GetSize() { return wt.scale_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>代入する座標位置
	void SetSize(Vector3 size) { wt.scale_ = size; }

	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns></returns>座標が中心のAABB
	AABB GetAABB();
	/// <summary>
	/// setter_当たり判定AABB
	/// </summary>
	/// <param name="aabb"></param>AABB
	void SetAABB(AABB aabb) { actorAABB = aabb; }

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
	/// getter_倒れた時
	/// </summary>
	/// <returns></returns>倒れたフラグ
	bool GetIsDead() { return isDead; }

	/// <summary>
	/// リアクション(拡大縮小)
	/// </summary>
	/// <param name="mosionOn"></param> リアクションフラグ
	/// <param name="scale"></param> どのくらい大きくするか
	/// <param name="maxTime"></param> リアクションタイマーの最大値
	void ScaleUpdate(bool* mosionOn, Vector3 scale,const float& maxTime);
	
	/// <summary>
	/// アニメーションのみを動かす関数(演出で使う)
	/// </summary>
	/// <param name="anser"></param>
	/// trueならアニメーションのみ / falseなら操作可能
	void IsPerformanceFlag(const bool& result) { isPerformance = result; }

	bool GetPerformanceMode() { return isPerformance; }


	/// <summary>
	/// getter_影の当たり判定
	/// </summary>
	/// <returns></returns>影のAABB
	AABB GetShadowAABB() { return shadow_->GetAABB(); }

	/// <summary>
	/// 影のみの更新処理
	/// </summary>
	void ShadowUpdate();


	/// <summary>
	/// 復活用の初期位置のデータ
	/// </summary>
	/// <param name="translate"></param>座標位置
	/// <param name="rotate"></param>各回転角度
	void SetInit_Position(Vector3 translate, Vector3 rotate) {
		init_point = translate;
		init_rotate = rotate;
	}

	/// <summary>
	/// 共通リスポーン(体力や初期位置などを戻す)
	/// </summary>
	void RespawnCommon();

	/// <summary>
	/// 体力の設定
	/// </summary>
	/// <param name="max"></param>最大体力
	void HP_Initialize(const uint32_t& max);

protected:

	WorldTransform wt;
	//当たり判定
	AABB actorAABB;

	//体力
	uint32_t maxHp;//最大値
	uint32_t hp;

	bool isDead = false;//倒れたフラグ

	//向き
	float direction = 90.0f;

	//地面判定
	bool isGround = false;
	//重力
	float grabity = 0.0f;

	//演出中フラグ
	bool isPerformance = false;

	//秒数時間
	const float kDeltaTime = 1.0f / 60.0f;

	///影
	std::unique_ptr<Shadow> shadow_;
	
	//ダメージのリアクション
	bool isDamageMosion = false;

	//ダメージリアクション
	float scaleTimer = 0.0f;
	Vector3 damageScale = { 0.1f, 0.1f, 0.1f };
	const Vector3 kDefaultScale = { 1,1,1 };//元の大きさ
	const float kDamageMaxTime = 0.14f;


	//初期位置保管
	Vector3 init_point;
	Vector3 init_rotate;

private:

};

