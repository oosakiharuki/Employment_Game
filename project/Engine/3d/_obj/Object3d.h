#pragma once
#include "MyMath.h"
#include "Model_obj.h"

//ComPtr
#include <wrl.h>
#include "d3d12.h"

#include "Camera.h"
#include "WorldTransform.h"

class Object3dCommon;
/// <summary>
/// .obj版のオブジェクト
/// </summary>
class Object3d
{
public:
	void Initialize();
	void Update(const WorldTransform& worldTransform);
	void Update();
	void Draw();
	void Draw(const std::string& textureData);

	void SetModel(Model_obj* model) { this->model_ = model; }
	void SetModelFile(const std::string& filePath);
	void LightSwitch(bool isLight);
	void SetColor(const Vector4& color);

private:
	Object3dCommon* object3dCommon_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_ = nullptr;

	//ライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightSphereResource_;
	//マテリアルにデータを書き込む
	DirectionalLight* directionalLightSphereData_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;

	//ポイントライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	//マテリアルにデータを書き込む
	PointLight* pointLightData_ = nullptr;

	//スポットライト用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;
	//マテリアルにデータを書き込む
	SpotLight* spotLightData_ = nullptr;
	Model_obj* model_ = nullptr;
	Camera* camera_ = nullptr;

	Material* material_;

	//ワールド行列
	Matrix4x4 worldMatrix_{};
};