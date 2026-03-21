#pragma once
#include "IStageObject.h"
#include "Object_glTF.h"
#include "Shadow.h"

/// <summary>
/// トゲボール[ギミック]
/// </summary>
class Needle : public IStageObject {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// setter_移動ルート
	/// </summary>
	/// <param name="nowPoint">現在の場所</param>
	/// <param name="pointS">ポイント始点</param>
	/// <param name="pointE">ポイント終点</param>
	void SetTravelRoute(const Vector3& nowPoint, const Vector3& pointS, const Vector3& pointE);


private:
	/// <summary>
	/// 移動
	/// </summary>
	void Move();
	/// <summary>
	/// 回転するアニメーション
	/// </summary>
	void AnimationRotate();

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

	std::unique_ptr<Object_glTF> objectNeedle_ = nullptr;

	Vector3 nowPoint_ = { 0,0,0 };
	Segment movePoint_{};

	Vector3 startPoint_ = { 0,0,0 };	
	Vector3 endPoint_ = { 0,0,0 };

	float timer_ = 0.0f;
	const float moveMaxTime_ = 5.0f;//遅くする

	float animationTimer_ = 0.0f;
	const float kAnimationTimeSpeed_ = 2.0f;


	std::unique_ptr<Shadow> shadow_;
};

