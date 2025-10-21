#pragma once
#include "Object3d.h"
#include "MyMath.h"

class IStageObject {
public:

	IStageObject();
	virtual ~IStageObject();
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	const Vector3& GetPosition() { return wt.translation_; }
	void SetPosition(const Vector3& position) { wt.translation_ = position; }

	const Vector3& GetRotation() { return wt.rotation_; }
	void SetRotation(const Vector3& rotation) { wt.rotation_ = rotation; }

	AABB GetAABB();
	void SetAABB(AABB aabb) { aabb_ = aabb; }

protected:

	std::unique_ptr<Object3d> object_ = nullptr;
	WorldTransform wt;
	AABB aabb_;
};