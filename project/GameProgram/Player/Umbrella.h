#pragma once
#include "Object_glTF.h"
#include "MyMath.h"

/// <summary>
/// 傘(発泡、守が使える)
/// </summary>
class Umbrella {
public:
	~Umbrella();
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
	/// getter_座標
	/// </summary>
	/// <returns></returns>現在の座標
	Vector3 GetTranslate() { return wt.translation_; }
	/// <summary>
	/// setter‗座標
	/// </summary>
	/// <param name="translate"></param>代入する座標
	void SetTranslate(Vector3 translate) { wt.translation_ = translate; }

	/// <summary>
	/// setter‗回転
	/// </summary>
	/// <param name="rotate"></param>代入する各回転角度
	void SetRotate(Vector3 rotate) { wt.rotation_ = rotate; }

	/// <summary>
	/// setter‗スケール
	/// </summary>
	/// <param name="scale"></param>代入するスケール
	void SetScale(Vector3 scale) { 
		wt.scale_ = scale;
		scaleTimer = 0.0f;
	}

	/// <summary>
	/// getter‗当たり判定AABB
	/// </summary>
	/// <returns></returns>座標が中心のAABB
	AABB GetAABB();
	/// <summary>
	/// シールドモード
	/// </summary>
	/// <param name="isShield"></param>trueはシールドモードに変更
	void ShieldMode(bool isShield) { isShieldMode = isShield; }

	/// <summary>
	/// 当たったリアクション(拡大縮小)
	/// </summary>
	/// <param name="mosionOn"></param>モーションフラグ
	/// <param name="scale"></param>どのくらい大きくするか
	/// <param name="maxTime"></param>リアクションタイマーの最大値
	void ScaleUpdate(bool* mosionOn, Vector3 scale, const float maxTime);
private:
	//オブジェクト設定
	std::unique_ptr<Object_glTF> object;
	WorldTransform wt;
	AABB umbrellaAABB;
	//シールドモード
	bool isShieldMode = false;
	//秒数時間
	const float deltaTime = 1.0f / 60.0f;
	//リアクションのタイマー
	float scaleTimer = 0.0f;
};

