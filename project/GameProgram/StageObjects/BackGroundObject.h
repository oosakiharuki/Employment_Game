#pragma once
#include "Object_glTF.h"

/// <summary>
/// 背景オブジェクト
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
	//背景オブジェクト
	std::unique_ptr<EngineLayer::Object_glTF> object_ = nullptr;
	//大きさ
	static constexpr float kSize_ = 1.0f;
};

