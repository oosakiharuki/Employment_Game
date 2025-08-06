#include "Camera.h"
#include "WinApp.h"
using namespace MyMath;
Camera::Camera() {
	worldTransform_.Initialize();
	forY = 0.45f;
	aspect = float(WinApp::kClientWidth) / float(WinApp::kClientHeight);
	nearClip = 0.1f;
	farClip = 100.0f;
	worldMatrix = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	viewMatrix = Inverse(worldMatrix);
	projectionMatrix = MakePerspectiveFovMatrix(forY, aspect, nearClip, farClip);
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}
void Camera::Update() {
	worldTransform_.UpdateMatrix();

	worldMatrix = worldTransform_.matWorld_;
	viewMatrix = Inverse(worldMatrix);
	projectionMatrix = MakePerspectiveFovMatrix(forY, aspect, nearClip, farClip);
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}