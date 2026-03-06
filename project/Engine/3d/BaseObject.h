#pragma once
#include "MyMath.h"
#include "BaseModel.h"

//ComPtr
#include <wrl.h>
#include "d3d12.h"

#include "Camera.h"
#include "WorldTransform.h"
/// <summary>
/// オブジェクト(基盤クラス)
/// </summary>
class BaseObject
{
public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="worldTransform"></param>
	virtual void Update(WorldTransform& worldTransform) = 0;
	/// <summary>
	/// 更新処理()
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;
	/// <summary>
	/// 描画処理(テクスチャの変更あり)
	/// </summary>
	/// <param name="textureData">変更したいテクスチャのファイルパス</param>
	virtual void Draw(const std::string& textureData) = 0;
	/// <summary>
	/// モデルデータ
	/// </summary>
	/// <param name="filePath">モデル名</param>
	virtual void SetModelFile(const std::string& filePath) = 0;
	/// <summary>
	/// ライトスイッチ
	/// </summary>
	/// <param name="isLight">オンオフ</param>
	virtual void LightSwitch(bool isLight) = 0;
	/// <summary>
	/// settter_色
	/// </summary>
	/// <param name="color">設定する色</param>
	virtual void SetColor(const Vector4& color) = 0;

protected:
	
	/// <summary>
	/// カメラ初期化処理
	/// </summary>
	void InitCamera();

	/// <summary>
	/// ライト初期化処理
	/// </summary>
	void InitLight();

	/// <summary>
	/// ディレクショナルライト設定
	/// </summary>
	void CreateDirectionalLight();
	/// <summary>
	/// ポイントライト設定
	/// </summary>
	void CreatePointLight();
	/// <summary>
	/// スポットライト設定
	/// </summary>
	void CreateSpotLight();


	//カメラ用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	//カメラデータを書き込む
	CameraForGPU* cameraData_ = nullptr;

	//ライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightData_ = nullptr;

	//ポイントライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	//マテリアルにデータを書き込む
	PointLight* pointLightData_ = nullptr;

	//スポットライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;
	//マテリアルにデータを書き込む
	SpotLight* spotLightData_ = nullptr;

	Camera* camera_ = nullptr;

	//ワールド行列
	Matrix4x4 worldMatrix_{};

	//ライト初期値
	const Vector4 kDefaultColor_ = { 1,1,1,1 };
	const float kIntensity_ = 1.0f;
	//ディレクショナルライト初期値
	const Vector3 kDirectionalLightDirection_ = { 0.0f,-1.0f,0.0f };
	//ポイントライト初期値
	const Vector3 kPointLightPosition_ = { 0.0f,2.0f,0.0f };
	const float kPointLightRadius_ = 5.0f;
	const float kPointLightDecay_ = 1.0f;
	//スポットライト初期値
	const Vector3 kSpotLightPosition_ = { 2.0f,1.25f,0.0f };
	const float kSpotLightDistance_ = 70.0f;
	const Vector3 kSpotLightDirection_ = { -1.0f,-1.0f,0.0f };
	const float kSpotLightDecay_ = 2.0f;
	const float kCosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	const float kCosFalloffStart = std::cos(std::numbers::pi_v<float> / 4.0f);
};

