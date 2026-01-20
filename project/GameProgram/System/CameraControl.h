#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "MyMath.h"
#include <LevelEditor.h>
#include "UseEveryOne.h"
/// <summary>
/// カメラをコントロールする
/// </summary>
class CameraControl {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="camera"></param>現在のカメラ
	void Update(Camera* camera);

	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate">指定する座標</param>
	void SetTranslation(const Vector3& translate) { transform_.translate = translate; }
	/// <summary>
	/// setter_回転
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Vector3& rotation) { transform_.rotate = rotation; }
	/// <summary>
	/// setter_カメラ移動範囲
	/// </summary>
	/// <param name="left">左端</param>
	/// <param name="right">右端</param>
	void SetEndPoint(const Vector3& left,const Vector3& right);
	/// <summary>
	/// setter_
	/// </summary>
	/// <param name="pos"></param>
	void SetPlayerPosition(const Vector3& pos) { playerPos_ = pos; }
	/// <summary>
	/// ズーム開始(現在カメラ位置から特定位置まで)
	/// </summary>
	/// <param name="goal"></param>特定位置を設定
	void ZoomStart(const Vector3& goal);
	/// <summary>
	/// ズーム終了
	/// </summary>
	/// <returns></returns>trueなら終了した
	const bool ZoomEnd();
	/// <summary>
	/// 固定モード
	/// </summary>
	/// <param name="result"></param> trueでオン / falseでオフ
	void FixedMode(bool result) { isFixedMode_ = result; }
	/// <summary>
	/// 固定モードであるか
	/// </summary>
	/// <returns></returns>
	bool IsFixed() const { return isFixedMode_; }

	/// <summary>
	/// シェイクモード
	/// </summary>
	/// <param name="result"></param> trueでオン / falseでオフ
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
	void CameraSetting(const CameraInitData& data, bool fixed_Mode_);

	/// <summary>
	/// 対応するカメラを変更
	/// </summary>
	/// <param name="data">変更したいカメラデータ</param>
	/// <param name="fixed_Mode_">固定する場合</param>
	void CameraInterpolation(const CameraInitData& data, bool fixed_Mode_);

	void CameraStartPointPlayer(const CameraInitData& data, const Vector3& playerPosition);


	/// <summary>
	/// 十字キーで移動する
	/// </summary>
	void DebugMove();

	void CameraYFixed(bool result) { isCameraYFixed_ = result; }

private:
	/// <summary>
	/// 通常(動く)
	/// </summary>
	void Move();
	/// <summary>
	/// カメラズーム(カメラが寄る)
	/// </summary>
	void Zoom();
	/// <summary>
	/// シェイク(カメラが揺れる)
	/// </summary>
	void Shaking();

	/// <summary>
	/// imGuiの更新処理
	/// </summary>
	void ImGuiUpdate();

	//右端、左端

	//範囲上限
	Vector3 minEndPoint_;//最小
	Vector3 maxEndPoint_;//最大

	//ワールド座標系
	WorldTransform wt_;
	Transform transform_{};

	//プレイヤー位置
	Vector3 playerPos_;
	//前にもらったプレイヤー位置
	Vector3 prevPlayerPos_;
	//
	const float kInterpolationPointX_ = 8.0f;


	//固定する高さ
	const float kFixedY_ = 6.0f;

	//固定する値
	bool isFixedMode_ = false;

	//シェイク
	const float kShakeMaxTime_ = 0.25f;
	float shakeTimer_ = kShakeMaxTime_;
	Vector3 preTranslate_{};
	bool isShakeMode_ = false;
	const float kShakePower = 0.5f;

	//カメラズーム
	Segment cameraSegment_ = {};
	const float kMoveFrame_ = 1.0f / 40.0f;//移動フレーム

	float zoomTimer_ = 0.0f;
	//ズーム時間最大値
	const float kMaxZoomTime_ = 1.5f;
	//ズーム中
	bool isZoom_ = false;

	//imGui
	bool isFreeMode_ = false;

	float movePower_ = 0.2f;

	//Y座標は固定するフラグ
	bool isCameraYFixed_ = false;
	//カメラの変更
	bool isChangeCamera_ = false;

	Vector3 segmentExtreme = { 0.1f,0.1f,0.1f };//極限まで数字を近く
};