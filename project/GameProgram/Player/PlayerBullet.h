#pragma once
#include "Object3d.h"
#include "MyMath.h"

/// <summary>
/// プレイヤーの弾丸
/// </summary>
class PlayerBullet {
public:

	~PlayerBullet();

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
	/// setter_弾丸速度
	/// </summary>
	/// <param name="velocity">代入する弾丸速度</param>
	void SetVelocty(const Vector3& velocity) { velocity_ = velocity; }

	/// <summary>
	/// 弾がなくなるフラグ
	/// </summary>
	/// <returns>trueなら消滅</returns>
	bool IsDead() { return isDead_; }
	/// <summary>
	/// 弾が敵などに当たる
	/// 消える
	/// </summary>
	void IsHit() { isDead_ = true; }

	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns>座標位置の真ん中のAABB</returns>
	AABB GetAABB() const;

private:
	//オブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;
	WorldTransform wt_;
	Transform transform_{};
	//速さ
	Vector3 velocity_;

	//消えるまでの処理
	float deathTimer_ = 0.0f;
	const float kEndTime_ = 0.7f;
	bool isDead_ = false;

	const Vector3 kBulletSize_ = { 1,1,1 };

	AABB bulletAABB_;
};