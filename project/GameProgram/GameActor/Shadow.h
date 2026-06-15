#pragma once
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
	void Update(const Transform& transform);

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

	Transform transform_{};

	//プレイヤーの真下最大値
	const float kShadowMinY_ = 1000.0f;
	//プレイヤー横幅(-にすることで端なら少し影が映る)
	const float kShadowWidth_ = -0.0f;
	//色
	const Vector4 kColor_ = { 0,0,0,1 };

	//一番真下の値
	float minUnder_ = 0.0f;

	Vector3 actorPosition_{};
};