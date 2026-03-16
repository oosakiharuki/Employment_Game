#pragma once
#include "IStageObject.h"
#include "Object_glTF.h"
#include "Shadow.h"

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

	void SetTravelRoute(const Vector3& nowPoint, const Vector3& pointS, const Vector3& pointE);


private:

	void Move();

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
	const float moveMaxTime_ = 5.0f;//二秒かけて進む

	float animationTimer_ = 0.0f;
	const float kAnimationTimeSpeed_ = 2.0f;


	std::unique_ptr<Shadow> shadow_;
};

