#pragma once
#include "MyMath.h"
#include "Object3d.h"
#include "Particle.h"

#include "GLTFCommon.h"
#include "Object3dCommon.h"
#include "Shadow.h"
#include "Sprite.h"

#include "UseEveryOne.h"

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
	Vector3 GetTranslate() { return wt_.translation_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>代入する座標位置
	void SetTranslate(Vector3 translate) { wt_.translation_ = translate; }

	/// <summary>
	/// getter_ワールド座標系の位置
	/// </summary>
	/// <returns></returns>平行移動成分を搭載したプレイヤー座標
	Vector3 GetWorldPosition();

	/// <summary>
	/// getter_回転
	/// </summary>
	/// <returns></returns>現在の回転
	Vector3 GetRotate() { return wt_.rotation_; }
	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotate"></param>代入する各回転角度
	void SetRotate(Vector3 rotate) { wt_.rotation_ = rotate; }
	
	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns></returns>現在の座標位置
	Vector3 GetSize() { return wt_.scale_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>代入する座標位置
	void SetSize(Vector3 size) { wt_.scale_ = size; }

	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns></returns>座標が中心のAABB
	AABB GetAABB();
	/// <summary>
	/// setter_当たり判定AABB
	/// </summary>
	/// <param name="aabb"></param>AABB
	void SetAABB(AABB aabb) { actorAABB_ = aabb; }

	/// <summary>
	/// 地面判定フラグ変更
	/// </summary>
	/// <param name="result"></param>trueは地面 / falseは空中
	void IsGround(bool result);

	/// <summary>
	/// getter_地面判定
	/// </summary>
	/// <returns></returns>現在の地面判定
	bool GetIsGround() { return isGround_; }

	/// <summary>
	/// getter_倒れた時
	/// </summary>
	/// <returns></returns>倒れたフラグ
	bool GetIsDead() { return isDead_; }

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
	void IsPerformanceFlag(const bool& result) { isPerformance_ = result; }

	bool GetPerformanceMode() { return isPerformance_; }


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
		initTranslate_ = translate;
		initRotate_ = rotate;
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

	WorldTransform wt_;
	//当たり判定
	AABB actorAABB_;

	//体力
	uint32_t maxHp_;//最大値
	uint32_t hp_;

	bool isDead_ = false;//倒れたフラグ

	//向き(左右)
	const float kDirectionRight_ = 90.0f;
	const float kDirectionLeft_ = -90.0f;
	
	//最大角度(360度)
	const float kMaxAngle = 360.0f;

	//地面判定
	bool isGround_ = false;
	//重力
	float grabity_ = 0.0f;
	const float kGrabityPower_ = 0.01f;//重力の質量

	//演出中フラグ
	bool isPerformance_ = false;

	///影
	std::unique_ptr<Shadow> shadow_;
	
	//ダメージのリアクション
	bool isDamageMosion_ = false;

	//ダメージリアクション
	float scaleTimer_ = 0.0f;
	Vector3 damageScale_ = { 0.1f, 0.1f, 0.1f };
	const float kDamageMaxTime_ = 0.14f;


	//初期位置保管
	Vector3 initTranslate_;
	Vector3 initRotate_;

private:

};

