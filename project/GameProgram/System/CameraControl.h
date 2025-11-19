#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "MyMath.h"
#include <Levelediter.h>

class CameraControl {
public:

	void Initialize();
	void Update(Camera* camera);

	void SetTranslation(const Vector3& translate) { wt_.translation_ = translate; }
	void SetRotation(const Vector3& rotation) { wt_.rotation_ = rotation; }

	void SetEndPoint(const Vector3& left,const Vector3& right);
	
	void SetPlayerPosition(Vector3 pos) { playerPos_ = pos; }
	void ZoomStart(const Vector3& goal);

	bool MaxZoom();

	void FixedMode(bool result) { isFixedMode_ = result; }
	bool IsFixed() { return isFixedMode_; }


	void ShakeMode(bool result) { isShakeMode_ = result; }

	/// <summary>
	/// シェイク時間を元(最大値)に戻す
	/// </summary>
	void ResetShakeTime() { shakeTimer_ = kShakeMaxTime_; }

	/// <summary>
	/// カメラを設定する
	/// </summary>
	/// <param name="data"></param>レベルエディタのカメラデータ(cameraInit)
	/// <param name="fixed_Mode_"></param>trueでカメラを固定する
	void CameraSetting(const CameraInitData& data, const bool& fixed_Mode_);

private:

	void Move();

	void Zoom();

	void Shaking();

	//右端、左端
	Vector3 leftEndPoint_;
	Vector3 rightEndPoint_;

	WorldTransform wt_;
	
	Vector3 playerPos_;

	const float kFixedY_ = 6.0f;//固定する高さ

	bool isFixedMode_ = false;
	Vector3 fixedPos_;

	//シェイク
	const float kShakeMaxTime_ = 0.25f;
	float shakeTimer_ = kShakeMaxTime_;
	Vector3 preTranslate_{};
	bool isShakeMode_ = false;

	//カメラズーム
	Segment cameraSegment_ = {};
	float zoomTimer_ = 0.0f;
	const float kMaxZoomTime_ = 1.0f;
	bool isZoom_ = false;
	
	//imgui
	bool isFreeMode_ = false;
};