#pragma once
#include "Object3d.h"
#include "Object_glTF.h"
#include "MyMath.h"
#include "CollisionManager.h"

/// <summary>
/// 影
/// </summary>
class Shadow : public CollisionSource {
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
	void SetTranslate(const Vector3& position) { transform_.translate = position; }
	/// <summary>
	/// setter_サイズ
	/// </summary>
	/// <param name="scale"></param>代入する大きさ
	void SetScale(const Vector3& scale) { transform_.scale = scale; }

private:
	void OnCollision(CollisionSource* collision) override;

	std::unique_ptr<Object3d> object_;
	WorldTransform wt_;
	Transform transform_{};

	AABB shadowAABB_;
	//プレイヤーの真下最大値
	const float kShadowMinY_ = -1000.0f;
	//プレイヤー横幅
	const float kShadowWidth_ = 0.1f;
	//色
	const Vector4 kColor_ = { 0,0,0,1 };
};