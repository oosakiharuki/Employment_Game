#include "CameraControl.h"
#include <sstream>
#include <random>

using namespace MyMath;

void CameraControl::Initialize() {
	wt.Initialize();
	fixed_Mode = false;

}

void CameraControl::Update(Camera* camera) {

	//固定モードでないなら
	if (!fixed_Mode) {
		Move();
	}

	//ズーム
	Zoom();
	//シェイク
	Shaking();


	wt.UpdateMatrix();

	camera->SetRotate(wt.rotation_);
	camera->SetTranslate(wt.translation_);
	camera->Update();

}

void CameraControl::SetEndPoint(const Vector3& left, const Vector3& right){
	leftEndPoint = left;
	rightEndPoint = right;
}

void CameraControl::Move() {
	
	//X座標
	//プレイヤーが左端を超えたら
	if (playerPos.x <= leftEndPoint.x) {
		wt.translation_.x = leftEndPoint.x;
	}//プレイヤーが右端を超えたら
	else if (playerPos.x >= rightEndPoint.x) {
		wt.translation_.x = rightEndPoint.x;
	}//両端の間
	else {
		wt.translation_.x = playerPos.x;//プレイヤーX座標
	}
	 
	//Y座標
	if (playerPos.y >= fixedY) {
		wt.translation_.y = playerPos.y + fixedY;
	}

}

void CameraControl::Zoom() {
	if (isZoom) {

		if (zoomTimer < 1.0f) {
			zoomTimer += 1.0f / 60.0f;
		}
		else {
			zoomTimer = 1.0f;
		}

		wt.translation_ = cameraSegment.origin + (cameraSegment.diff - cameraSegment.origin) * zoomTimer;
	}
}

void CameraControl::Shaking() {

	if (ShakeTimer <= 0.0f) {
		shake_Mode = false;
		//カメラ位置を戻す
		wt.translation_ = preTranslate;
		return;
	}
	else if (shake_Mode) {
		//シェイクする前に元々のカメラ位置を設定
		if (ShakeTimer == ShakeMaxTime) {
			//シェイク前のカメラ位置
			preTranslate = wt.translation_;
		}


		ShakeTimer -= 1.0f / 60.0f;

		std::random_device seed;
		std::mt19937 random(seed());

		std::uniform_real_distribution<float> yure(-0.5f, 0.5f);

		//上下左右にシェイク(z軸は関係ない)
		wt.translation_ += Vector3{ yure(random), yure(random), 0.0f };
	}
}


bool CameraControl::MaxZoom() {
	if (zoomTimer >= 1.0f) {
		return true;
	}
	return false;
}

void CameraControl::ZoomStart(const Vector3& goal) {
	cameraSegment.origin = wt.translation_;
	cameraSegment.diff = goal;
	isZoom = true;
}

void CameraControl::CameraSetting(const CameraInitData& data, const bool& fixed_Mode_) {
	//座標と回転
	wt.rotation_ = data.rotation;
	wt.translation_ = data.translation;

	//カメラの最小/最大地点
	leftEndPoint = wt.translation_ + data.Point1;
	rightEndPoint = wt.translation_ + data.Point2;

	fixed_Mode = fixed_Mode_;
}
