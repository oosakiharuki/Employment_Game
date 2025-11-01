#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "MyMath.h"
#include <Levelediter.h>

class CameraControl {
public:

	void Initialize();
	void Update(Camera* camera);

	void SetTranslation(const Vector3& translate) { wt.translation_ = translate; }
	void SetRotation(const Vector3& rotation) { wt.rotation_ = rotation; }

	void SetEndPoint(const Vector3& left,const Vector3& right);
	
	void SetPlayerPosition(Vector3 pos) { playerPos = pos; }
	void ZoomStart(const Vector3& goal);

	bool MaxZoom();

	void FixedMode(bool result) { fixed_Mode = result; }
	bool IsFixed() { return fixed_Mode; }


	void ShakeMode(bool result) { shake_Mode = result; }

	/// <summary>
	/// シェイク時間を元(最大値)に戻す
	/// </summary>
	void ResetShakeTime() { ShakeTimer = ShakeMaxTime; }

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
	Vector3 leftEndPoint;
	Vector3 rightEndPoint;

	WorldTransform wt;
	
	Vector3 playerPos;

	float fixedY = 6.0f;//固定する高さ

	bool fixed_Mode = false;
	Vector3 fixedPos;

	//シェイク
	const float ShakeMaxTime = 0.25f;
	float ShakeTimer = ShakeMaxTime;
	Vector3 preTranslate{};
	bool shake_Mode = false;

	//カメラズーム
	Segment cameraSegment = {};
	float zoomTimer = 0.0f;
	bool isZoom = false;
	//Camera
};