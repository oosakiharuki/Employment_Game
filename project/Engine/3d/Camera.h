#pragma once
#include "MyMath.h"
#include "WorldTransform.h"

class Camera {
public:
	Camera();
	void Update();
	void SetRotate(const Vector3& rotate) { worldTransform_.rotation_ = rotate; }
	void SetTranslate(const Vector3& translate) { worldTransform_.translation_ = translate; }
	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

	void SetForY(const float& ForY) { forY = ForY; }
	void SetAspectRatio(const float aspectRatio) { aspect = aspectRatio; }
	void SetNearClip(const float Clip) { nearClip = Clip; }
	void SetFarClip(const float Clip) { farClip = Clip; }
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }
	const Vector3& GetRotate() const { return worldTransform_.rotation_; }
	const Vector3& GetTranslate() const { return worldTransform_.translation_; }
private:
	WorldTransform worldTransform_;
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	float forY;
	float aspect;
	float nearClip;
	float farClip;
	Matrix4x4 viewProjectionMatrix;
};