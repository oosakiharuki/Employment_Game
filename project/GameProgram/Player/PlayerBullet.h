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
	/// <returns></returns>現在の座標位置
	Vector3 GetTranslate() { return wt.translation_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>代入する座標位置
	void SetTranslate(Vector3 translate) { wt.translation_ = translate; }

	/// <summary>
	/// setter_弾丸速度
	/// </summary>
	/// <param name="velocity"></param>代入する弾丸速度
	void SetVelocty(Vector3 velocity) { velocity_ = velocity; }

	/// <summary>
	/// 弾がなくなるフラグ
	/// </summary>
	/// <returns></returns>trueなら消滅
	bool IsDead() { return isDead; }
	/// <summary>
	/// 弾が敵などに当たる
	/// 消える
	/// </summary>
	void IsHit() { isDead = true; }

	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns></returns>座標位置の真ん中のAABB
	AABB GetAABB();

private:
	std::unique_ptr<Object3d> object = nullptr;
	WorldTransform wt;
	Vector3 velocity_;

	//消えるまでの処理
	float deathTimer = 0.0f;
	const float endTime = 0.7f;
	bool isDead = false;
	
	AABB bulletAABB;

};