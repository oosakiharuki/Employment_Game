#pragma once
#include"Vector3.h"
#include "Matrix4x4.h"

/// <summary>
/// ワールド座標
/// </summary>
class WorldTransform{
public:
	
	Vector3 scale_;
	Vector3 rotation_ ;
	Vector3 translation_ ;
	Matrix4x4 matWorld_;
	const WorldTransform* parent_ = nullptr;

	WorldTransform() = default;
	~WorldTransform() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void UpdateMatrix();
private:

};