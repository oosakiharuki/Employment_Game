#pragma once
#include "Object3d.h"
#include "Object_glTF.h"
#include "MyMath.h"

/// <summary>
/// 影
/// </summary>
class Shadow {
public:
	~Shadow();
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
	/// setter_座標位置
	/// </summary>
	/// <param name="position"></param>代入する座標位置
	void SetTranslate(Vector3 position) { wt_.translation_ = position; }
	/// <summary>
	/// setter_サイズ
	/// </summary>
	/// <param name="scale"></param>代入する大きさ
	void SetScale(Vector3 scale) { wt_.scale_ = scale; }

	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns></returns>座標位置が真ん中のAABB
	AABB GetAABB();

private:
	std::unique_ptr<Object3d> object_;
	WorldTransform wt_;
	AABB shadowAABB_;
	//プレイヤーの真下最大値
	const float kShadowMinY_ = -1000.0f;
	//プレイヤー横幅
	const float kShadowWidth_ = 0.1f;
	//色
	const Vector4 kColor_ = { 0,0,0,1 };
};