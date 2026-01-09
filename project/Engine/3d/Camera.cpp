#include "Camera.h"
#include "WinApp.h"
using namespace MyMath;
Camera::Camera() {
	worldTransform_.Initialize();//ワールド座標更新
	//Transform更新処理
	transform_ = worldTransform_.UpdateTransform();

	forY_ = 0.45f;//カメラの
	aspect_ = float(WinApp::kClientWidth_) / float(WinApp::kClientHeight_);//アスペクト比(windowsの幅/高さ)
	nearClip_ = 0.1f;//カメラの描画範囲(手前-z)
	farClip_ = 1000.0f;//カメラの描画範囲(奥 z)
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);//ワールド座標行列
	viewMatrix_ = Inverse(worldMatrix_);//ビュー行列
	projectionMatrix_ = MakePerspectiveFovMatrix(forY_, aspect_, nearClip_, farClip_);//
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);//ビュープロジェクション行列
}
void Camera::Update() {
	worldMatrix_ = worldTransform_.GetMatWorld();
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(forY_, aspect_, nearClip_, farClip_);
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
	
	worldTransform_.UpdateMatrix(transform_);
}
