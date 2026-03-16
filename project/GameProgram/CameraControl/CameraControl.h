#pragma once
#include "Camera.h"
#include "MyMath.h"
#include "LevelEditor.h"
#include "UseEveryOne.h"
#include "Player.h"

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
	/// <param name="camera">現在のカメラ</param>
	/// <param name="player">プレイヤー情報</param>
	void Update(Camera* camera,Player* player = nullptr);

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
	/// setter_プレイヤーの座標位置
	/// </summary>
	/// <param name="pos">プレイヤー座標</param>
	void SetPlayerPosition(const Vector3& pos) { playerPos_ = pos; }
	/// <summary>
	/// ズーム終了
	/// </summary>
	/// <returns>trueなら終了した</returns>
	const bool ZoomEnd();
	/// <summary>
	/// 固定モード
	/// </summary>
	/// <param name="result">trueでオン / falseでオフ</param> 
	void FixedMode(bool result) { isFixedMode_ = result; }
	/// <summary>
	/// 固定モードであるか
	/// </summary>
	/// <returns>trueなら固定モードである</returns>
	bool IsFixed() const { return isFixedMode_; }

	/// <summary>
	/// シェイクモード
	/// </summary>
	/// <param name="result">trueでオン / falseでオフ</param> 
	void ShakeMode(bool result) { isShakeMode_ = result; }

	/// <summary>
	/// シェイク時間を元(最大値)に戻す
	/// </summary>
	void ResetShakeTime() { shakeTimer_ = kShakeMaxTime_; }

	/// <summary>
	/// カメラを設定する
	/// </summary>
	/// <param name="data">レベルエディタのカメラデータ(cameraInit)</param>
	/// <param name="fixed_Mode_">trueでカメラを固定する</param>
	void CameraSetting(const CameraInitData& data, bool fixed_Mode_);

	/// <summary>
	/// リスポーン時、チェックポイントから出てくる
	/// </summary>
	/// <param name="data">メインカメラのデータ</param>
	void CameraSettingCheckPoint(const CameraInitData& data);

	/// <summary>
	/// 対応するカメラを変更
	/// </summary>
	/// <param name="data">変更したいカメラデータ</param>
	/// <param name="fixed_Mode_">固定する場合</param>
	void CameraInterpolation(const CameraInitData& data, bool fixed_Mode_);
	/// <summary>
	/// プレイヤーのポイント
	/// </summary>
	/// <param name="data">カメラデータ</param>
	/// <param name="playerPosition">プレイヤーの座標</param>
	void CameraStartPointPlayer(const CameraInitData& data, const Vector3& playerPosition);


	/// <summary>
	/// 十字キーで移動する
	/// </summary>
	void DebugMove();
	/// <summary>
	/// カメラのy軸移動を停止(ボス戦で使用)
	/// </summary>
	void CameraYFixed() { isCameraYFixed_ = true; }

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
	/// ズーム開始(現在カメラ位置から特定位置まで)
	/// </summary>
	/// <param name="goal">特定位置を設定</param>
	void ZoomStart(const Vector3& goal);
	/// <summary>
	/// シェイク(カメラが揺れる)
	/// </summary>
	void Shaking();

	/// <summary>
	/// imGuiの更新処理
	/// </summary>
	void ImGuiUpdate();

	/// <summary>
	/// セグメント
	/// </summary>
	/// <param name="origin">始点</param>
	/// <param name="diff">終点</param>
	void SetSegment(const Vector3& origin, const Vector3& diff);

	/// <summary>
	/// 変更時の補間処理
	/// </summary>
	void ChangeInterpolation();

	//右端、左端

	//範囲上限
	Vector3 minEndPoint_;//最小
	Vector3 maxEndPoint_;//最大

	//座標
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
	float interpolation = 0.0f;

	float moveFrame_;//使用する補間フレーム
	const float kLittleFront_ = kInterpolationPointX_ + 1.0f;//プレイヤーの少し前方向
	const float kMoveFrameStrong_ = 1.0f / 60.0f;//補間フレーム強(補間が長い)
	const float kMoveFrameMedium_ = 1.0f / 20.0f;//補間フレーム中
	const float kMoveFrameWeek_ = 1.0f / 10.0f;//補間フレーム弱(補間が短い)

	float zoomTimer_ = 0.0f;
	//ズーム時間最大値
	const float kMaxZoomTime_ = 1.5f;
	//ズーム中
	bool isZoom_ = false;

	//imGui
	bool isFreeMode_ = false;

	float movePower_ = 1.0f;

	//Y座標は固定するフラグ
	bool isCameraYFixed_ = false;
	//カメラの変更
	bool isChangeCamera_ = false;

	Vector3 segmentExtreme = { 0.1f,0.1f,0.1f };//極限まで数字を近く

	//ズームするときのプレイヤーと少し離れた位置
	const Vector3 kPlayerAwayPos_ = { 0, 1, -15.0f };
};