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
	/// <summary>
	/// 
	/// </summary>
	/// <param name="objectName"></param>
	void Initialize(const std::string objectName);
	/// <summary>
	/// 
	/// </summary>
	void Update();
	/// <summary>
	/// 
	/// </summary>
	void Draw();
	/// <summary>
	/// 
	/// </summary>
	/// <param name="transform"></param>
	void SetTransform(const Transform& transform) { transform_ = transform; }
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	const Transform& GetTransform() { return transform_; }
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	const std::string& GetObjectName() { return objectName_; }
	/// <summary>
	/// 
	/// </summary>
	/// <param name="changeObjectName"></param>
	void ChangeObject(const std::string& changeObjectName) { baseObject_->ChangeAnimation(changeObjectName); }
	/// <summary>
	/// 
	/// </summary>
	void LightOn();

private:
	WorldTransform wt_;
	Transform transform_;

	std::unique_ptr<Object_glTF> baseObject_;

	std::string objectName_;
};

