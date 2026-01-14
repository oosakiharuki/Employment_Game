#pragma once
#include "MyMath.h"
#include "Object_gltf.h"

/// <summary>
/// 見た目専用オブジェクト
/// (PlayerのモデルだがPlayer処理を行わない、Titleなどで使う)
/// </summary>
class VisualActor {
public:

	VisualActor();
	~VisualActor();

	void Initialize(const std::string objectName);

	void Update();

	void Draw();

	void SetTransform(const Transform& transform) { transform_ = transform; }

	const Transform& GetTransform() { return transform_; }

	const std::string& GetObjectName() { return objectName_; }

	void ChangeObject(const std::string& changeObjectName) { baseObject_->ChangeAnimation(changeObjectName); }

private:
	WorldTransform wt_;
	Transform transform_;

	std::unique_ptr<Object_glTF> baseObject_;

	std::string objectName_;
};

