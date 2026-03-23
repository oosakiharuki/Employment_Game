#pragma once
#include "IStageObject.h"
#include "Object_glTF.h"

/// <summary>
/// 動く足場[ギミック]
/// </summary>
class MoveGround : public IStageObject{
public:
	~MoveGround() override;
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

	Vector3 pointA_{};
	Vector3 pointB_{};

	Segment segment_{};


	Vector3 nowPoint_ = { 0,0,0 };
	Segment movePoint_{};

	Vector3 startPoint_ = { 0,0,0 };
	Vector3 endPoint_ = { 0,0,0 };


	float timer_ = 0.0f;
	const float moveMaxTime_ = 5.0f;//遅くする
};

