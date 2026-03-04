#pragma once
#include <memory>
#include "Object3d.h"

class Guide {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Transform& transform,const std::string& textureFileName);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	std::unique_ptr<Object3d> object_ = nullptr;
	WorldTransform wt_;
	Transform transform_;

	std::string textureFileName_;
	std::string textureFilePath_;
};

