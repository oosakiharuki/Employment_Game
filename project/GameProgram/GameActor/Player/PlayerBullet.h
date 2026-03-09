#pragma once
#include "Object3d.h"
#include "MyMath.h"
#include "CollisionManager.h"

/// <summary>
/// プレイヤーの弾丸
/// </summary>
class PlayerBullet : public CollisionSource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
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
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
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

	//オブジェクト
	std::unique_ptr<Object3d> object_ = nullptr;
	WorldTransform wt_;
	Transform transform_{};
	//速さ
	Vector3 velocity_;
	Vector3 kBulletEndPoint_;

	//消えるまでの処理
	float deathTimer_ = 0.0f;
	const float kEndTime_ = 0.7f;
	bool isDead_ = false;

	const Vector3 kBulletSize_ = { 1,1,1 };

	AABB bulletAABB_;
};