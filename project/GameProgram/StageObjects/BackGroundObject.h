#pragma once
#include "Object_glTF.h"

/// <summary>
/// 背景オブジェクト(skydome)
/// </summary>
class BackGroundObject
{
public:
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

private:

	std::unique_ptr<Object_glTF> object_ = nullptr;
	WorldTransform wt_;
	Transform transform_{};

	const float kSize_ = 1.0f;
};

