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
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// setter_座標位置の(Y軸はなし)
	/// </summary>
	/// <param name="position">代入する座標位置</param>
	void SetTranslate(const Vector3& position) { actorPosition_ = position; }
	/// <summary>
	/// setter_サイズ
	/// </summary>
	/// <param name="scale">代入する大きさ</param>
	void SetScale(const Vector3& scale) { transform_.scale = scale; }

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

	std::unique_ptr<Object_glTF> object_;
	WorldTransform wt_;
	Transform transform_{};

	//プレイヤーの真下最大値
	const float kShadowMinY_ = 1000.0f;
	//プレイヤー横幅
	const float kShadowWidth_ = 0.1f;
	//色
	const Vector4 kColor_ = { 0,0,0,1 };

	//一番真下の値
	float minUnder_ = 0.0f;

	Vector3 actorPosition_{};
};