#include "Camera.h"
#include "WinApp.h"
using namespace MyMath;
Camera::Camera() {
	worldTransform_.Initialize();
	forY_ = 0.45f;
	aspect_ = float(WinApp::kClientWidth_) / float(WinApp::kClientHeight_);
	nearClip_ = 0.1f;
	farClip_ = 1000.0f;
	worldMatrix_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(forY_, aspect_, nearClip_, farClip_);
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}
void Camera::Update() {
	worldTransform_.UpdateMatrix();

	worldMatrix_ = worldTransform_.matWorld_;
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(forY_, aspect_, nearClip_, farClip_);
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}
