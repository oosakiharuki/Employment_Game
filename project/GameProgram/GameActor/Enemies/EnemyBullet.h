#pragma once
#include "Object3d.h"
#include "MyMath.h"
#include "CollisionManager.h"

class Player;

/// <summary>
/// 敵の弾丸
/// </summary>
class EnemyBullet : public CollisionSource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
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
	/// <returns>現在の座標位置</returns>
	const Vector3& GetTranslate() const { return transform_.translate; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate">指定する座標</param>
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	
	/// <summary>
	/// setter_弾丸速度、方向
	/// </summary>
	/// <param name="velocity"></param>FireBulletで設定したvelocity
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

	/// <summary>
	/// 消滅フラグ
	/// </summary>
	/// <returns></returns>
	bool IsDead() { return isDead_; }

	/// <summary>
	/// getter_パリィフラグ
	/// </summary>
	/// <returns>現在のパリィフラグ</returns>
	bool GetIsParry() { return isParry; }
	/// <summary>
	/// getter_direction
	/// </summary>
	/// <returns>プレイヤーと弾の距離</returns>
	const Vector3& GetDistance() const { return distance; }
	/// <summary>
	/// setter_プレイヤー
	/// </summary>
	/// <param name="player">プレイヤークラス</param>
	void SetPlayer(Player* player) { player_ = player; }

private:	
	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;
	/// <summary>
	/// 当たり判定をとるタイプかをチェック
	/// </summary>
	/// <param name="collisionType">相手の当たり判定タイプ</param>
	/// <returns>該当するタイプがあるなら true</returns>
	bool TypeCheckUp(const CollisionTypes& collisionType) override;

	//
	std::unique_ptr<Object3d> object = nullptr;
	WorldTransform wt_;
	Transform transform_{};
	//弾丸速度
	Vector3 velocity_;

	//消えるまでの処理
	float deathTimer = 0.0f;
	const float kEndTime = 2.0f;
	bool isDead_ = false;

	//当たり判定
	AABB bulletAABB;
	const Vector3 kBulletSize_ = { 1.0f ,1.0f ,1.0f };

	//パリィされたら敵のほうにダメージになる
	bool isParry = false;

	//当たり所
	Vector3 distance;

	//プレイヤークラス
	Player* player_ = nullptr;
};

