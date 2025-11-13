#pragma once
#include "Object3d.h"
#include "MyMath.h"

class Player;

/// <summary>
/// 敵の弾丸
/// </summary>
class EnemyBullet {
public:

	~EnemyBullet();

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
	/// <param name="translate"></param>指定する座標
	void SetTranslate(Vector3 translate) { wt.translation_ = translate; }
	
	/// <summary>
	/// setter_弾丸速度、方向
	/// </summary>
	/// <param name="velocity"></param>FireBulletで設定したvelocity
	void SetVelocty(Vector3 velocity) { velocity_ = velocity; }

	/// <summary>
	/// 消滅フラグ
	/// </summary>
	/// <returns></returns>
	bool IsDead() { return isDead; }
	/// <summary>
	/// 当たったフラグ
	/// </summary>
	void IsHit();
	/// <summary>
	/// getter_当たり判定AABB
	/// </summary>
	/// <returns></returns>
	AABB GetAABB();

	/// <summary>
	/// 跳ね返す(パリィ)処理
	/// </summary>
	void Pari_Mode();
	/// <summary>
	/// getter_パリィフラグ
	/// </summary>
	/// <returns></returns>現在のパリィフラグ
	bool GetIsPari() { return isPari; }
	/// <summary>
	/// getter_direction
	/// </summary>
	/// <returns></returns>プレイヤーと弾の距離
	Vector3 GetDistance() { return distance; }
	/// <summary>
	/// setter_プレイヤー
	/// </summary>
	/// <param name="player"></param>使っているプレイヤー
	void SetPlayer(Player* player) { player_ = player; }

private:
	std::unique_ptr<Object3d> object = nullptr;
	WorldTransform wt;
	Vector3 velocity_;

	//消えるまでの処理
	float deathTimer = 0.0f;
	const float kEndTime = 2.0f;
	bool isDead = false;

	//秒数時間
	const float kDeltaTime = 1.0f / 60.0f;

	//当たり判定
	AABB bulletAABB;

	//パリィされたら敵のほうにダメージになる
	bool isPari = false;

	//当たり所
	Vector3 distance;

	//プレイヤークラス
	Player* player_ = nullptr;
};

