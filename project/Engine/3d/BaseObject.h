#pragma once
#include "MyMath.h"
#include "BaseModel.h"

//ComPtr
#include <wrl.h>
#include "d3d12.h"

#include "Camera.h"
#include "WorldTransform.h"

class BaseObject
{
public:
	virtual void Initialize() = 0;
	virtual void Update(WorldTransform& worldTransform) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Draw(const std::string& textureData) = 0;


	virtual void SetModelFile(const std::string& filePath) = 0;
	virtual void LightSwitch(bool isLight) = 0;
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
};

